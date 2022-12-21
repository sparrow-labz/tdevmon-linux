#include "pch.h"
#include "CmdLine.h"

//..............................................................................

CmdLine::CmdLine() {
	m_verb = CmdLineVerb_Help;
	m_flags = 0;
}

//..............................................................................

bool
CmdLineParser::onValue(const sl::StringRef& value) {
	err::setFormatStringError("unrecognized command line entry: %s", value.sz());
	return false;
}

bool
CmdLineParser::onSwitch(
	SwitchKind switchKind,
	const sl::StringRef& value
) {
#if (_AXL_OS_LINUX)
	bool result;
	dm_IoctlDesc ioctlDesc;
#endif

	switch (switchKind) {
	case CmdLineSwitch_Help:
		m_cmdLine->m_verb = CmdLineVerb_Help;
		break;

	case CmdLineSwitch_Version:
		m_cmdLine->m_verb = CmdLineVerb_Version;
		break;

	case CmdLineSwitch_Stats:
		m_cmdLine->m_verb = CmdLineVerb_Stats;
		break;

	case CmdLineSwitch_Monitor:
		m_cmdLine->m_verb = CmdLineVerb_Monitor;
		m_cmdLine->m_deviceName = value;
		break;

	case CmdLineSwitch_FileNameWildcard:
		m_cmdLine->m_fileNameWildcard = value;
		break;

#if (_AXL_OS_WIN)
	case CmdLineSwitch_ListClassDevices:
		m_cmdLine->m_verb = CmdLineVerb_ListClassDevices;
		m_cmdLine->m_deviceClassName = value;
		break;

	case CmdLineSwitch_ListAllDevices:
		m_cmdLine->m_verb = CmdLineVerb_ListAllDevices;
		break;

	case CmdLineSwitch_DeviceInfo:
		m_cmdLine->m_verb = CmdLineVerb_DeviceInfo;
		m_cmdLine->m_deviceClassName = value;
		break;

	case CmdLineSwitch_Install:
		m_cmdLine->m_verb = CmdLineVerb_Install;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_Uninstall:
		m_cmdLine->m_verb = CmdLineVerb_Uninstall;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_Reinstall:
		m_cmdLine->m_verb = CmdLineVerb_Reinstall;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_CreateCoreService:
		m_cmdLine->m_verb = CmdLineVerb_CreateService;
		m_cmdLine->m_serviceKind = dm::ServiceKind_Core;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_DeleteCoreService:
		m_cmdLine->m_verb = CmdLineVerb_DeleteService;
		m_cmdLine->m_serviceKind = dm::ServiceKind_Core;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_StartCoreService:
		m_cmdLine->m_verb = CmdLineVerb_StartCoreService;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_StopCoreService:
		m_cmdLine->m_verb = CmdLineVerb_StopCoreService;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_RestartCoreService:
		m_cmdLine->m_verb = CmdLineVerb_RestartCoreService;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_CreatePnpService:
		m_cmdLine->m_verb = CmdLineVerb_CreateService;
		m_cmdLine->m_serviceKind = dm::ServiceKind_Pnp;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_DeletePnpService:
		m_cmdLine->m_verb = CmdLineVerb_DeleteService;
		m_cmdLine->m_serviceKind = dm::ServiceKind_Pnp;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_Force:
		m_cmdLine->m_flags |= CmdLineFlag_Force;
		break;

	case CmdLineSwitch_AddPnpFilter:
		m_cmdLine->m_verb = CmdLineVerb_AddPnpFilter;
		m_cmdLine->m_deviceClassName = value;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_RemovePnpFilter:
		m_cmdLine->m_verb = CmdLineVerb_RemovePnpFilter;
		m_cmdLine->m_deviceClassName = value;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_RemoveAllPnpFilters:
		m_cmdLine->m_verb = CmdLineVerb_RemoveAllPnpFilters;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_CheckAccess:
		m_cmdLine->m_verb = CmdLineVerb_CheckAccess;
		break;

	case CmdLineSwitch_ShowSd:
		m_cmdLine->m_verb = CmdLineVerb_ShowSd;
		break;

	case CmdLineSwitch_ClearSd:
		m_cmdLine->m_verb = CmdLineVerb_SetAccess;
		m_cmdLine->m_flags |= CmdLineFlag_ClearSd | CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_SetDefaultSd:
		m_cmdLine->m_verb = CmdLineVerb_SetAccess;
		m_cmdLine->m_flags |= CmdLineFlag_SetDefaultSd | CmdLineFlag_AdminRequired;
		break;

	case CmdLineSwitch_AddDenyAce:
		m_cmdLine->m_verb = CmdLineVerb_SetAccess;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		m_cmdLine->m_denyList.insertTail(value);
		break;

	case CmdLineSwitch_AddAllowAce:
		m_cmdLine->m_verb = CmdLineVerb_SetAccess;
		m_cmdLine->m_flags |= CmdLineFlag_AdminRequired;
		m_cmdLine->m_allowList.insertTail(value);
		break;

	case CmdLineSwitch_AccountInfo:
		m_cmdLine->m_verb = CmdLineVerb_AccountInfo;
		m_cmdLine->m_accountName = value;
		break;

	case CmdLineSwitch_ListAllAccounts:
		m_cmdLine->m_verb = CmdLineVerb_ListAllAccounts;
		break;
#	ifdef _AXL_DEBUG
	case CmdLineSwitch_Test:
		m_cmdLine->m_verb = CmdLineVerb_Test;
		break;
#	endif
#elif (_AXL_OS_LINUX)
	case CmdLineSwitch_Unhook:
		m_cmdLine->m_verb = CmdLineVerb_Unhook;
		m_cmdLine->m_deviceName = value;
		break;

	case CmdLineSwitch_Stop:
		m_cmdLine->m_verb = CmdLineVerb_Stop;
		break;

	case CmdLineSwitch_IoctlDesc:
		result = parseIoctlDesc(&ioctlDesc, value);
		if (!result)
			return false;

		m_cmdLine->m_ioctlDescArray.append(ioctlDesc);
		break;
#endif

	case CmdLineSwitch_MachineIface:
		m_cmdLine->m_flags |= CmdLineFlag_MachineIface;
		break;
	}

	return true;
}

#if (_AXL_OS_LINUX)
bool
CmdLineParser::parseIoctlDesc(
	dm_IoctlDesc* ioctlDesc,
	const sl::StringRef& string
) {
	size_t i = string.find(':');
	if (i == -1) {
		err::setFormatStringError("missing ':' in ioctl descriptor '%s'", string.sz());
		return false;
	}

	sl::StringRef codeString = string.getLeftSubString(i);
	sl::StringRef descString = string.getSubString(i + 1);

	char* end;
	ioctlDesc->m_code = strtoul(codeString.cp(), &end, 0);
	if (end == codeString.cp()) {
		err::setFormatStringError("invalid ioctl code '%s'", codeString.sz());
		return false;
	}

	AXL_TODO("add arg size fields for variable-length args support")

	ioctlDesc->m_argFixedSize = strtoul(descString.cp(), &end, 0);
	if (end == descString.cp()) {
		err::setFormatStringError("invalid ioctl arg-size '%s'", descString.sz());
		return false;
	}

	ioctlDesc->m_flags = 0;
	ioctlDesc->m_argSizeFieldOffset = 0;
	return true;
}
#endif

//..............................................................................
