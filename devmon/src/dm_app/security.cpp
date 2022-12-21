#include "pch.h"
#include "security.h"
#include "error.h"
#include "CmdLine.h"
#include "AccessMgr.h"
#include "dm_win_Service.h"
#include "dm_win_Monitor.h"

#pragma warning(disable: 4996) // 'GetVersionExW': was declared deprecated

//..............................................................................

// OWNER: SYSTEM
// GROUP: SYSTEM
// ALLOW: FILE_ALL_ACCESS for Administrators

static uint8_t g_defaultSd[] = {
	0x01, 0x00, 0x04, 0x90, 0x34, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x14, 0x00, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00,
	0xff, 0x01, 0x1f, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x20, 0x00, 0x00, 0x00,
	0x20, 0x02, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x12, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x12, 0x00, 0x00, 0x00,
};

//..............................................................................

bool
isAdmin() {
	bool result;

	sys::win::AccessToken token;
	result = token.openProcessToken(TOKEN_QUERY);
	if (!result)
		return false;

	OSVERSIONINFOEXW versionInfo = { 0 };
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

	typedef
	LONG
	WINAPI
	RtlGetVersionFunc(RTL_OSVERSIONINFOW* versionInfo);

	HMODULE ntdll = ::GetModuleHandleW(L"ntdll.dll");
	RtlGetVersionFunc* rtlGetVersion = (RtlGetVersionFunc*) ::GetProcAddress(ntdll, "RtlGetVersion");
	rtlGetVersion ?
		rtlGetVersion((RTL_OSVERSIONINFOW*) &versionInfo) :
		::GetVersionExW((OSVERSIONINFOW*) &versionInfo);

	bool isVistaOrHigher =
		versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
		versionInfo.dwMajorVersion >= 6;

	if (isVistaOrHigher) {
		TOKEN_ELEVATION_TYPE elevationType;

		dword_t actualSize;
		bool result = token.getTokenInformation(TokenElevationType, &elevationType, sizeof(elevationType), &actualSize);
		if (!result)
			return false;

		if (elevationType == TokenElevationTypeFull)
			return true;
		else if (elevationType == TokenElevationTypeLimited)
			return false;

		// not a split token, fall through
	}

	sys::win::Sid adminGroupSid;
	result = adminGroupSid.create(&sys::win::g_ntSidAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS);
	if (!result)
		return false;

	return token.isMemberOf(adminGroupSid);
}

bool
readSecurityDescriptor(
	HKEY hkey,
	sl::Array<char>* buffer
) {
	long result;

	dword_t size = 0;
	result = ::RegQueryValueExW(hkey, DM_CORE_SERVICE_REG_PARAM_SDW, NULL, NULL, NULL, &size);

	if (result != ERROR_SUCCESS) {
		err::setError(result);
		return false;
	}

	result = buffer->setCount(size);
	if (!result)
		return false;

	result = ::RegQueryValueExW(hkey, DM_CORE_SERVICE_REG_PARAM_SDW, NULL, NULL, (byte_t*)buffer->p(), &size);
	if (result != ERROR_SUCCESS) {
		err::setError(result);
		return false;
	}

	return true;
}

bool
writeSecurityDescriptor(
	HKEY hkey,
	const void* buffer,
	size_t size
) {
	long result = ::RegSetValueExW(hkey, DM_CORE_SERVICE_REG_PARAM_SDW, NULL, REG_BINARY, (const byte_t*) buffer, size);
	if (result != ERROR_SUCCESS) {
		err::setError(result);
		return false;
	}

	return true;
}

int
checkAccess() {
	bool result;

	dm::Monitor monitor;
	result = monitor.open();
	if (!result) {
		printf("Can't open %s: %s\n", DM_MASTER_DEVICE_NAMEA, err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	result = monitor.checkAccess();
	if (result)
		printf("Access granted\n");
	else
		printf("%s\n", err::getLastErrorDescription().sz());

	return Error_Success;
}

void
printAccountNameSid(const SID* sid0) {
	sys::win::Sid sid(sid0);

	sl::String accountName = sid.lookupAccountSid();
	if (!accountName.isEmpty())
		printf("%s ", accountName.sz());

	printf("%s\n", sid.getString().sz());
}

int
printSecurityDescriptor() {
	bool result;

	HKEY hkey;
	result = dm::openCoreServiceParameterRegKey(&hkey, KEY_READ);
	if (!result) {
		printf("Can't open " DM_CORE_SERVICE_NAMEA " registry key: %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	sys::win::RegKeyHandle key = hkey;

	sl::Array<char> sdBuffer;
	result = readSecurityDescriptor(key, &sdBuffer);
	if (!result || !IsValidSecurityDescriptor(sdBuffer)) {
		printf("No valid security descriptor set (defaults will be used by " DM_CORE_DRIVER_FILE_NAMEA ")\n");
		return Error_IoFailure;
	}

	const SECURITY_DESCRIPTOR_RELATIVE* sd = (const SECURITY_DESCRIPTOR_RELATIVE*) sdBuffer.cp();
	const SID* ownerSid = sd->Owner ? (SID*)((char*)sd + sd->Owner) : NULL;
	const SID* groupSid = sd->Group ? (SID*)((char*)sd + sd->Group) : NULL;
	const ACL* acl = sd->Dacl ? (ACL*)((char*)sd + sd->Dacl) : NULL;

	printf("OWNER: ");

	if (ownerSid)
		printAccountNameSid(ownerSid);
	else
		printf("<not-set>\n");

	printf("GROUP: ");

	if (groupSid)
		printAccountNameSid(groupSid);
	else
		printf("<not-set>\n");

	if (!acl || !acl->AceCount) {
		printf("DACL is empty\n");
		return Error_Success;
	}

	const ACCESS_ALLOWED_ACE* ace = (ACCESS_ALLOWED_ACE*)(acl + 1);

	for (WORD i = 0; i < acl->AceCount; i++) {
		if (ace->Header.AceType == ACCESS_ALLOWED_ACE_TYPE ||
			ace->Header.AceType == ACCESS_DENIED_ACE_TYPE) {
			printf(ace->Header.AceType == ACCESS_ALLOWED_ACE_TYPE ? "ALLOW: " : "DENY:  ");
			printAccountNameSid((SID*) &ace->SidStart);
		}

		ace = (ACCESS_ALLOWED_ACE*)((char*)ace + ace->Header.AceSize);
	}

	return Error_Success;
}

inline
sl::String
getUserComment(const sl::StringRef& accountName) {
	USER_INFO_20* user;
	NET_API_STATUS status = ::NetUserGetInfo(NULL, sl::String_w(accountName), 20, (BYTE**) &user);
	sl::String comment = status == ERROR_SUCCESS ? user->usri20_comment : NULL;
	::NetApiBufferFree(user);
	return comment;
}

sl::String
getGroupComment(const sl::StringRef& accountName) {
	LOCALGROUP_INFO_1* group;
	NET_API_STATUS status = ::NetLocalGroupGetInfo(NULL, sl::String_w(accountName), 1, (BYTE**) &group);
	sl::String comment = status == ERROR_SUCCESS ? group->lgrpi1_comment : NULL;
	::NetApiBufferFree(group);
	return comment;
}

int
printAccountInfo(const sl::StringRef& accountName) {
	bool result;
	sys::win::Sid sid;
	SID_NAME_USE sidType;

	result = _strnicmp(accountName.sz(), "S-", 2) == 0 && sid.parse (accountName);
	if (!result) {
		result = sid.lookupAccountName(accountName);
		if (!result) {
			printf("Can't find account: %s\n", accountName.sz());
			return Error_NameNotFount;
		}
	}

	sl::String sidString = sid.getString();
	sl::String properAccountName;
	result = sid.lookupAccountSid(&properAccountName, &sidType);
	if (!result) {
		printf("Can't get information on %s: %s\n", sidString.sz(), err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	static const char* sidTypeStringTable[] = {
		"<unkown-account-type>",
		"User",
		"Group",
		"Domain",
		"Alias",
		"Well-known group",
		"Deleted account",
		"Invalid",
		"Unknown",
		"Computer",
		"Label"
	};

	const char* sidTypeString = (size_t)sidType < countof(sidTypeStringTable) ?
		sidTypeStringTable[sidType] :
		sidTypeStringTable[0];

	printf("%s: %s %s\n", sidTypeString, properAccountName.sz(), sidString.sz());

	sl::String comment;
	switch (sidType) {
	case SidTypeUser:
		comment = getUserComment(properAccountName);
		break;

	case SidTypeGroup:
	case SidTypeWellKnownGroup:
	case SidTypeAlias:
		comment = getGroupComment(properAccountName);
		break;
	};

	if (!comment.isEmpty())
		printf("%s\n", comment.sz());

	return Error_Success;
}

int
listAllAccounts() {
	bool result;
	NET_API_STATUS status;
	sys::win::Sid sid;
	sl::String string;

	printf("Well-known groups:\n");

	static SID wellKnownGroups[] = {
		{ SID_REVISION, 1, SECURITY_WORLD_SID_AUTHORITY, SECURITY_WORLD_RID },
		{ SID_REVISION, 1, SECURITY_LOCAL_SID_AUTHORITY, SECURITY_LOCAL_RID },
		{ SID_REVISION, 1, SECURITY_NT_AUTHORITY, SECURITY_NETWORK_RID },
		{ SID_REVISION, 1, SECURITY_NT_AUTHORITY, SECURITY_INTERACTIVE_RID },
		{ SID_REVISION, 1, SECURITY_NT_AUTHORITY, SECURITY_SERVICE_RID },
		{ SID_REVISION, 1, SECURITY_NT_AUTHORITY, SECURITY_ANONYMOUS_LOGON_RID },
		{ SID_REVISION, 1, SECURITY_NT_AUTHORITY, SECURITY_AUTHENTICATED_USER_RID },
		{ SID_REVISION, 1, SECURITY_NT_AUTHORITY, SECURITY_TERMINAL_SERVER_RID },
		{ SID_REVISION, 1, SECURITY_NT_AUTHORITY, SECURITY_LOCAL_SYSTEM_RID },
		{ SID_REVISION, 1, SECURITY_NT_AUTHORITY, SECURITY_LOCAL_SERVICE_RID },
		{ SID_REVISION, 1, SECURITY_NT_AUTHORITY, SECURITY_NETWORK_SERVICE_RID },
	};

	for (size_t i = 0; i < countof(wellKnownGroups); i++) {
		sid = &wellKnownGroups[i];

		result = sid.lookupAccountSid(&string);
		if (!result)
			continue;

		printf("%s ", string.sz());

		sid.getString(&string);
		printf("%s\n", string.sz());
	}

	printf("\nGroups:\n");

	DWORD_PTR groupEnum = 0;

	do {
		LOCALGROUP_INFO_1* groupBuffer;
		DWORD totalCount;
		DWORD readCount;

		status = ::NetLocalGroupEnum(
			NULL,
			1,
			(BYTE**) &groupBuffer,
			MAX_PREFERRED_LENGTH,
			&readCount,
			&totalCount,
			&groupEnum
		);

		if (status != ERROR_SUCCESS && status != ERROR_MORE_DATA)
			break;

		LOCALGROUP_INFO_1* p = groupBuffer;
		LOCALGROUP_INFO_1* end = p + readCount;
		for (; p < end; p++) {
			result = sid.lookupAccountName(p->lgrpi1_name);
			if (!result)
				continue;

			sid.getString(&string);
			printf("%S %s\n", p->lgrpi1_name, string.sz());
		}

		::NetApiBufferFree(groupBuffer);
	} while (status == ERROR_MORE_DATA);

	printf("\nUsers:\n");

	DWORD userEnum = 0;

	do {
		USER_INFO_20* userBuffer;
		DWORD totalCount;
		DWORD readCount;

		status = ::NetUserEnum(
			NULL,
			20,
			0,
			(BYTE**) &userBuffer,
			MAX_PREFERRED_LENGTH,
			&readCount,
			&totalCount,
			&userEnum
		);

		if (status != ERROR_SUCCESS && status != ERROR_MORE_DATA)
			break;

		USER_INFO_20* p = userBuffer;
		USER_INFO_20* pEnd = p + readCount;
		for (; p < pEnd; p++) {
			result = sid.lookupAccountName(p->usri20_name);
			if (!result)
				continue;

			sid.getString(&string);
			printf("%S %s\n", p->usri20_name, string.sz());
		}

		::NetApiBufferFree(userBuffer);
	} while (status == ERROR_MORE_DATA);

	return Error_Success;
}

int
setAccess(
	const sl::ConstBoxList<sl::String>& denyList,
	const sl::ConstBoxList<sl::String>& allowList,
	int flags
) {
	bool result;

	AccessMgr accessMgr;

	HKEY hkey;
	result = dm::openCoreServiceParameterRegKey(&hkey, KEY_READ | KEY_WRITE);
	if (!result) {
		printf("Can't open " DM_CORE_SERVICE_NAMEA " registry key: %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	sys::win::RegKeyHandle key = hkey;
	sl::Array<char> sdBuffer;

	if (flags & CmdLineFlag_SetDefaultSd) {
		accessMgr.loadSecurityDescriptor(&g_defaultSd, sizeof(g_defaultSd));
	} else if (!(flags & CmdLineFlag_ClearSd)) {
		result = readSecurityDescriptor(key, &sdBuffer);
		if (result)
			accessMgr.loadSecurityDescriptor(sdBuffer, sdBuffer.getCount());
	}

	sl::ConstBoxIterator<sl::String> it = denyList.getHead();
	for (; it; it++) {
		const sl::String& accountName = *it;
		result = accessMgr.addAccount(accountName, ACCESS_DENIED_ACE_TYPE);
		if (!result) {
			printf("Can't deny %s: %s\n", accountName.sz(), err::getLastErrorDescription().sz());
			return Error_NameNotFount;
		}
	}

	it = allowList.getHead();
	for (; it; it++) {
		const sl::String& accountName = *it;
		result = accessMgr.addAccount(accountName, ACCESS_ALLOWED_ACE_TYPE);
		if (!result) {
			printf("Can't allow %s: %s\n", accountName.sz(), err::getLastErrorDescription().sz());
			return Error_NameNotFount;
		}
	}

	result =
		accessMgr.createSecurityDescriptor(&sdBuffer) &&
		writeSecurityDescriptor(key, sdBuffer, sdBuffer.getCount());

	if (!result) {
		printf("Can't set security descriptor: %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	printSecurityDescriptor();
	return Error_Success;
}

//..............................................................................
