#include "pch.h"
#include "AccessMgr.h"

//..............................................................................

AccessMgr::AccessMgr() {
	m_totalSidSize = 0;

	// owner/group is SYSTEM by default

	m_ownerSid.create(&sys::win::g_ntSidAuthority, 1, SECURITY_LOCAL_SYSTEM_RID);
	m_groupSid.create(&sys::win::g_ntSidAuthority, 1, SECURITY_LOCAL_SYSTEM_RID);
}

void
AccessMgr::clear() {
	m_accountList.clear();
	m_sidMap.clear();
	m_totalSidSize = 0;
}

bool
AccessMgr::createSecurityDescriptor(sl::Array<char>* buffer) {
	normalizeAccountList();

	size_t count = m_accountList.getCount();

	size_t aceHdrSize = offsetof(ACCESS_ALLOWED_ACE, SidStart);
	size_t aclSize = sizeof(ACL) + count * aceHdrSize + m_totalSidSize;
	size_t ownerSidSize = m_ownerSid.getSize();
	size_t groupSidSize = m_groupSid.getSize();
	size_t sdSize = sizeof(SECURITY_DESCRIPTOR_RELATIVE) + aclSize + ownerSidSize + groupSidSize;

	bool result = buffer->setCount(sdSize);
	if (!result)
		return false;

	SECURITY_DESCRIPTOR_RELATIVE* sd = (SECURITY_DESCRIPTOR_RELATIVE*)buffer->p();
	ACL* acl = (ACL*)(sd + 1);
	SID* ownerSid = (SID*)((char*)acl + aclSize);
	SID* groupSid = (SID*)((char*)ownerSid + ownerSidSize);

	memset(sd, 0, sdSize);

	sd->Revision = SECURITY_DESCRIPTOR_REVISION;
	sd->Control = SE_SELF_RELATIVE | SE_DACL_PRESENT | SE_DACL_PROTECTED;
	sd->Dacl = (DWORD)((char*)acl - (char*)sd);
	sd->Owner = ownerSidSize ? (DWORD)((char*)ownerSid - (char*)sd) : 0;
	sd->Group = groupSidSize ? (DWORD)((char*)groupSid - (char*)sd) : 0;

	acl->AclRevision = ACL_REVISION;
	acl->AclSize = (WORD)aclSize;
	acl->AceCount = (WORD)count;

	ACCESS_ALLOWED_ACE* ace = (ACCESS_ALLOWED_ACE*)(acl + 1);

	sl::Iterator<Account> it = m_accountList.getHead();
	for (; it; it++) {
		Account* account = *it;
		size_t sidSize = account->m_sid.getSize();

		ace->Header.AceType = account->m_aceType;
		ace->Header.AceSize = (WORD)(aceHdrSize + sidSize);
		ace->Mask = FILE_ALL_ACCESS;

		memcpy(&ace->SidStart, account->m_sid, sidSize);

		ace = (ACCESS_ALLOWED_ACE*)((char*)ace + ace->Header.AceSize);
	}

	if (ownerSidSize)
		memcpy(ownerSid, m_ownerSid, ownerSidSize);

	if (groupSidSize)
		memcpy(groupSid, m_groupSid, groupSidSize);

	return true;
}

bool
AccessMgr::loadSecurityDescriptor(
	const void* sdBuffer,
	size_t sdSize
) {
	clear();

	SECURITY_DESCRIPTOR_RELATIVE* sd = (SECURITY_DESCRIPTOR_RELATIVE*)sdBuffer;
	bool_t isValid = IsValidSecurityDescriptor(sd);
	if (!isValid)
		return err::fail(err::SystemErrorCode_InvalidParameter);

	if (sd->Owner)
		m_ownerSid = (SID*)((char*)sd + sd->Owner);

	if (sd->Group)
		m_ownerSid = (SID*)((char*)sd + sd->Group);

	ACL* acl = (ACL*)((char*)sd + sd->Dacl);
	ACCESS_ALLOWED_ACE* ace = (ACCESS_ALLOWED_ACE*)(acl + 1);

	for (size_t i = 0; i < acl->AceCount; i++) {
		addAccount((SID*) &ace->SidStart, ace->Header.AceType);
		ace = (ACCESS_ALLOWED_ACE*)((char*)ace + ace->Header.AceSize);
	}

	return true;
}

bool
AccessMgr::addAccount(
	const sl::StringRef& accountName,
	byte_t aceType
) {
	sys::win::Sid sid;

	bool result = _strnicmp(accountName.sz(), "S-", 2) == 0 && sid.parse (accountName);
	if (!result) {
		result = sid.lookupAccountName(accountName);
		if (!result)
			return false;
	}

	addAccount(sid, aceType);
	return true;
}

void
AccessMgr::addAccount(
	const sys::win::Sid& sid,
	byte_t aceType
) {
	sl::HashTableIterator<const SID*, Account*> it = m_sidMap.visit(sid);
	if (it->m_value) {
		it->m_value->m_aceType = aceType;
		return;
	}

	Account* account = AXL_MEM_NEW(Account);
	account->m_sid = sid;
	account->m_aceType = aceType;
	m_accountList.insertTail(account);
	it->m_value = account;
	m_totalSidSize += sid.getSize();
}

void
AccessMgr::normalizeAccountList() {
	sl::List<Account> allowList;

	sl::Iterator<Account> it = m_accountList.getHead();
	while (it) {
		sl::Iterator<Account> nextIt = it.getNext();
		Account* account = *it;

		if (account->m_aceType == ACCESS_ALLOWED_ACE_TYPE) {
			m_accountList.remove(account);
			allowList.insertTail(account);
		}

		it = nextIt;
	}

	m_accountList.insertListTail(&allowList);
}

//..............................................................................
