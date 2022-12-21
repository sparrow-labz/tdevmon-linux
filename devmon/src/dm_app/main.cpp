#include "pch.h"
#include "CmdLine.h"
#include "version.h"
#include "error.h"
#include "stats.h"
#include "monitor.h"

#if (_AXL_OS_WIN)
#	include "service.h"
#	include "pnp.h"
#	include "security.h"
#	include "AccessMgr.h"

#	include "dm_win_Monitor.h"
#elif (_AXL_OS_LINUX)
#	include "dm_lnx_Monitor.h"
#endif

//..............................................................................

int
printUsage() {
	printVersion(false);

	sl::String helpString = CmdLineSwitchTable::getHelpString();
	printf("Usage: tdevmon [<options>...]\n%s", helpString.sz());

	return Error_Success;
}

#if (_AXL_OS_WIN && _AXL_DEBUG)
int
test() {
	printf("Testing...\n");

	dm::Monitor monitor;
	bool result = monitor.open() && monitor.test();
	if (!result) {
		printf("Error: %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	return Error_Success;
}
#elif (_AXL_OS_LINUX)
int
unhookDevice(const sl::StringRef& deviceName) {
	printf("Unhooking device: %s...\n", deviceName.sz());

	dm::Monitor monitor;
	bool result = monitor.open() && monitor.unhook(deviceName);
	if (!result) {
		printf("Error: %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	return Error_Success;
}

int
stopDriver() {
	printf("Removing all hooks and stopping device monitor...\n");

	dm::Monitor monitor;
	bool result = monitor.open() && monitor.stop();
	if (!result) {
		printf("Error: %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	return Error_Success;
}
#endif

//..............................................................................

int
main(
	int argc,
	char* argv[]
) {
#if _AXL_OS_POSIX
	setvbuf(stdout, NULL, _IOLBF, 1024);
#endif

	g::getModule()->setTag("dm_app");

	if (argc < 2) {
		printUsage();
		return Error_Success;
	}

	CmdLine cmdLine;
	CmdLineParser parser(&cmdLine);

	bool result = parser.parse(argc, argv);
	if (!result) {
		printf("Invalid command line: %s\n", err::getLastErrorDescription().sz());
		return Error_InvalidCmdLine;
	}

#if (_AXL_OS_WIN)
	if ((cmdLine.m_flags & CmdLineFlag_AdminRequired) && !isAdmin()) {
		printf("This operation requires administrative privilegies\n");
		return Error_AdminRequired;
	}
#endif

	switch (cmdLine.m_verb) {
	case CmdLineVerb_Help:
		return printUsage();

	case CmdLineVerb_Version:
		return printVersion(true);

	case CmdLineVerb_Stats:
		return printStats();

#if (_AXL_OS_WIN)
	case CmdLineVerb_Monitor:
		return (cmdLine.m_flags & CmdLineFlag_MachineIface) ?
			monitorDevice_mi(cmdLine.m_deviceName, cmdLine.m_fileNameWildcard) :
			monitorDevice(cmdLine.m_deviceName, cmdLine.m_fileNameWildcard);

	case CmdLineVerb_Install:
		return install();

	case CmdLineVerb_Uninstall:
		return uninstall((cmdLine.m_flags & CmdLineFlag_Force) != 0);

	case CmdLineVerb_Reinstall:
		return reinstall((cmdLine.m_flags & CmdLineFlag_Force) != 0);

	case CmdLineVerb_CreateService:
		return ::createService(cmdLine.m_serviceKind);

	case CmdLineVerb_DeleteService:
		return ::deleteService(cmdLine.m_serviceKind);

	case CmdLineVerb_StartCoreService:
		return startCoreService();

	case CmdLineVerb_StopCoreService:
		return stopCoreService((cmdLine.m_flags & CmdLineFlag_Force) != 0);

	case CmdLineVerb_RestartCoreService:
		return restartCoreService((cmdLine.m_flags & CmdLineFlag_Force) != 0);

	case CmdLineVerb_ListClassDevices:
		return listClassDevices(cmdLine.m_deviceClassName);

	case CmdLineVerb_ListAllDevices:
		return listAllDevices();

	case CmdLineVerb_DeviceInfo:
		return printDeviceInfo(cmdLine.m_deviceClassName);

	case CmdLineVerb_AddPnpFilter:
		return installPnpFilter(cmdLine.m_deviceClassName, true);

	case CmdLineVerb_RemovePnpFilter:
		return installPnpFilter(cmdLine.m_deviceClassName, false);

	case CmdLineVerb_RemoveAllPnpFilters:
		return removeAllPnpFilters();

	case CmdLineVerb_CheckAccess:
		return checkAccess();

	case CmdLineVerb_ShowSd:
		return printSecurityDescriptor();

	case CmdLineVerb_SetAccess:
		return setAccess(
			cmdLine.m_denyList,
			cmdLine.m_allowList,
			cmdLine.m_flags
		);

	case CmdLineVerb_AccountInfo:
		return printAccountInfo(cmdLine.m_accountName);

	case CmdLineVerb_ListAllAccounts:
		return listAllAccounts();

#	ifdef _AXL_DEBUG
	case CmdLineVerb_Test:
		return test();
#	endif
#elif (_AXL_OS_LINUX)
	case CmdLineVerb_Monitor:
		return (cmdLine.m_flags & CmdLineFlag_MachineIface) ?
			monitorDevice_mi(
				cmdLine.m_deviceName,
				cmdLine.m_fileNameWildcard,
				cmdLine.m_ioctlDescArray.cp(),
				cmdLine.m_ioctlDescArray.getCount()
			) :
			monitorDevice(
				cmdLine.m_deviceName,
				cmdLine.m_fileNameWildcard,
				cmdLine.m_ioctlDescArray.cp(),
				cmdLine.m_ioctlDescArray.getCount()
			);

	case CmdLineVerb_Unhook:
		return unhookDevice(cmdLine.m_deviceName);

	case CmdLineVerb_Stop:
		return stopDriver();
#endif

	default:
		printf("Invalid command line verb: %d\n", cmdLine.m_verb);
		return Error_InvalidCmdLine;
	}
}

//..............................................................................
