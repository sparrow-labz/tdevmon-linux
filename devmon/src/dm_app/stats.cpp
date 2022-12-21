#include "pch.h"
#include "CmdLine.h"
#include "version.h"
#include "error.h"

#if (_AXL_OS_WIN)
#	include "dm_win_DeviceMgr.h"
#	include "dm_win_Monitor.h"
#	include "dm_win_PnpBeacon.h"
#elif (_AXL_OS_LINUX)
#	include "dm_lnx_Monitor.h"
#	define DM_CORE_DRIVER_FILE_NAMEA DM_DRIVER_FILE_NAME
#	define DM_MASTER_DEVICE_NAMEA    DM_DEVICE_NAME
namespace dm {
typedef Monitor DeviceMgr;
} // namespace dm

#endif

//..............................................................................

void
printDriverVersion(dm::DeviceMgr* deviceMgr) {
	uint_t version = deviceMgr->getDriverVersion();
	sl::String description = deviceMgr->getDriverDescription();
	printf(
		"%s v%d.%d.%d\n",
		description.sz(),
		HIWORD(version),
		HIBYTE(LOWORD(version)),
		LOBYTE(LOWORD(version))
	);

	sl::String buildTime = deviceMgr->getDriverBuildTime();

	printf(
		"Built on: %s\n",
		buildTime.sz()
	);
}

int
printVersion(bool extendedInfo) {
	bool result;

	printf(
		"Tibbo Device Monitor v%d.%d.%d (%s%s)\n",
		VERSION_MAJOR,
		VERSION_MINOR,
		VERSION_REVISION,
		AXL_CPU_STRING,
		AXL_DEBUG_SUFFIX
	);

	if (!extendedInfo)
		return Error_Success;

	printf(
		"Built on: %s %s\n\n",
		__DATE__,
		__TIME__
	);

	dm::Monitor monitor;
	result = monitor.open();
	if (!result) {
		printf(DM_CORE_DRIVER_FILE_NAMEA " version information is not available: %s\n", err::getLastErrorDescription().sz());
	} else {
		printDriverVersion(&monitor);
		printf("\n");
	}
#if (_AXL_OS_WIN)
	dm::PnpBeacon beacon;
	result = beacon.open();
	if (!result)
		printf(DM_PNP_DRIVER_FILE_NAMEA " version information is not available: %s\n", err::getLastErrorDescription().sz());
	else
		printDriverVersion(&beacon);
#endif

	return Error_Success;
}

#if (_AXL_OS_WIN)
bool
printStats(dm::DeviceMgr* deviceMgr) {
	static const char* deviceTypeStringTable[FILE_DEVICE_PMI + 1] = {
		"<undefined>",                     // 0x00000000
		"FILE_DEVICE_BEEP",                // 0x00000001
		"FILE_DEVICE_CD_ROM",              // 0x00000002
		"FILE_DEVICE_CD_ROM_FILE_SYSTEM",  // 0x00000003
		"FILE_DEVICE_CONTROLLER",          // 0x00000004
		"FILE_DEVICE_DATALINK",            // 0x00000005
		"FILE_DEVICE_DFS",                 // 0x00000006
		"FILE_DEVICE_DISK",                // 0x00000007
		"FILE_DEVICE_DISK_FILE_SYSTEM",    // 0x00000008
		"FILE_DEVICE_FILE_SYSTEM",         // 0x00000009
		"FILE_DEVICE_INPORT_PORT",         // 0x0000000a
		"FILE_DEVICE_KEYBOARD",            // 0x0000000b
		"FILE_DEVICE_MAILSLOT",            // 0x0000000c
		"FILE_DEVICE_MIDI_IN",             // 0x0000000d
		"FILE_DEVICE_MIDI_OUT",            // 0x0000000e
		"FILE_DEVICE_MOUSE",               // 0x0000000f
		"FILE_DEVICE_MULTI_UNC_PROVIDER",  // 0x00000010
		"FILE_DEVICE_NAMED_PIPE",          // 0x00000011
		"FILE_DEVICE_NETWORK",             // 0x00000012
		"FILE_DEVICE_NETWORK_BROWSER",     // 0x00000013
		"FILE_DEVICE_NETWORK_FILE_SYSTEM", // 0x00000014
		"FILE_DEVICE_NULL",                // 0x00000015
		"FILE_DEVICE_PARALLEL_PORT",       // 0x00000016
		"FILE_DEVICE_PHYSICAL_NETCARD",    // 0x00000017
		"FILE_DEVICE_PRINTER",             // 0x00000018
		"FILE_DEVICE_SCANNER",             // 0x00000019
		"FILE_DEVICE_SERIAL_MOUSE_PORT",   // 0x0000001a
		"FILE_DEVICE_SERIAL_PORT",         // 0x0000001b
		"FILE_DEVICE_SCREEN",              // 0x0000001c
		"FILE_DEVICE_SOUND",               // 0x0000001d
		"FILE_DEVICE_STREAMS",             // 0x0000001e
		"FILE_DEVICE_TAPE",                // 0x0000001f
		"FILE_DEVICE_TAPE_FILE_SYSTEM",    // 0x00000020
		"FILE_DEVICE_TRANSPORT",           // 0x00000021
		"FILE_DEVICE_UNKNOWN",             // 0x00000022
		"FILE_DEVICE_VIDEO",               // 0x00000023
		"FILE_DEVICE_VIRTUAL_DISK",        // 0x00000024
		"FILE_DEVICE_WAVE_IN",             // 0x00000025
		"FILE_DEVICE_WAVE_OUT",            // 0x00000026
		"FILE_DEVICE_8042_PORT",           // 0x00000027
		"FILE_DEVICE_NETWORK_REDIRECTOR",  // 0x00000028
		"FILE_DEVICE_BATTERY",             // 0x00000029
		"FILE_DEVICE_BUS_EXTENDER",        // 0x0000002a
		"FILE_DEVICE_MODEM",               // 0x0000002b
		"FILE_DEVICE_VDM",                 // 0x0000002c
		"FILE_DEVICE_MASS_STORAGE",        // 0x0000002d
		"FILE_DEVICE_SMB",                 // 0x0000002e
		"FILE_DEVICE_KS",                  // 0x0000002f
		"FILE_DEVICE_CHANGER",             // 0x00000030
		"FILE_DEVICE_SMARTCARD",           // 0x00000031
		"FILE_DEVICE_ACPI",                // 0x00000032
		"FILE_DEVICE_DVD",                 // 0x00000033
		"FILE_DEVICE_FULLSCREEN_VIDEO",    // 0x00000034
		"FILE_DEVICE_DFS_FILE_SYSTEM",     // 0x00000035
		"FILE_DEVICE_DFS_VOLUME",          // 0x00000036
		"FILE_DEVICE_SERENUM",             // 0x00000037
		"FILE_DEVICE_TERMSRV",             // 0x00000038
		"FILE_DEVICE_KSEC",                // 0x00000039
		"FILE_DEVICE_FIPS",                // 0x0000003a
		"FILE_DEVICE_INFINIBAND",          // 0x0000003b
		"<undefined>",                     // 0x0000003c
		"<undefined>",                     // 0x0000003d
		"FILE_DEVICE_VMBUS",               // 0x0000003e
		"FILE_DEVICE_CRYPT_PROVIDER",      // 0x0000003f
		"FILE_DEVICE_WPD",                 // 0x00000040
		"FILE_DEVICE_BLUETOOTH",           // 0x00000041
		"FILE_DEVICE_MT_COMPOSITE",        // 0x00000042
		"FILE_DEVICE_MT_TRANSPORT",        // 0x00000043
		"FILE_DEVICE_BIOMETRIC",           // 0x00000044
		"FILE_DEVICE_PMI",                 // 0x00000045
	};

	bool result;

	sl::BoxList<dm::DeviceInfo> deviceInfoList;
	result = deviceMgr->getDeviceInfoList(&deviceInfoList);
	if (!result)
		return false;

	size_t totalConnectionCount = 0;
	size_t totalRequestCount = 0;

	dm::DeviceMgrKind deviceMgrKind = deviceMgr->getDeviceMgrKind();
	if (deviceMgrKind == dm::DeviceMgrKind_PnpBeacon) {
		sl::BoxIterator<dm::DeviceInfo> it = deviceInfoList.getHead();
		for (; it; it++) {
			dm::DeviceInfo* deviceInfo = it.p();

			const char* deviceTypeString = deviceInfo->m_ntDeviceType < countof(deviceTypeStringTable) ?
				deviceTypeStringTable[deviceInfo->m_ntDeviceType] :
				deviceTypeStringTable[0];

			printf(
				"Device name:    %s\n"
				"Flags:          0x%02x - %s\n"
				"NT device type: 0x%02x - %s\n"
				"\n",
				deviceInfo->m_deviceName.sz(),
				deviceInfo->m_flags,
				(deviceInfo->m_flags & dm_DeviceFlag_PnpDevice) ?
					(deviceInfo->m_flags & dm_DeviceFlag_FilterConnected) ? "PNP (connected)" : "PNP (disconnected)" :
					(deviceInfo->m_flags & dm_DeviceFlag_RemotePnpFilter) ? "remote PNP filter" : "legacy filter",
				deviceInfo->m_ntDeviceType,
				deviceTypeString
			);

			totalConnectionCount += deviceInfo->m_connectionCount;
			totalRequestCount += deviceInfo->m_requestCount;
		}

		printf("Total devices:  %d\n", deviceInfoList.getCount());
	} else {
		sl::BoxIterator<dm::DeviceInfo> it = deviceInfoList.getHead();
		for (; it; it++) {
			dm::DeviceInfo* deviceInfo = it.p();

			const char* deviceTypeString = deviceInfo->m_ntDeviceType < countof(deviceTypeStringTable) ?
				deviceTypeStringTable[deviceInfo->m_ntDeviceType] :
				deviceTypeStringTable[0];

			printf(
				"Device name:      %s\n"
				"Flags:            0x%02x - %s\n"
				"NT device type:   0x%02x - %s\n"
				"Connections:      %d\n"
				"Pending requests: %d\n"
				"\n",
				deviceInfo->m_deviceName.sz(),
				deviceInfo->m_flags,
				(deviceInfo->m_flags & dm_DeviceFlag_PnpDevice) ?
					(deviceInfo->m_flags & dm_DeviceFlag_FilterConnected) ? "PNP (connected)" : "PNP (disconnected)" :
					(deviceInfo->m_flags & dm_DeviceFlag_RemotePnpFilter) ? "remote PNP filter" : "legacy filter",
				deviceInfo->m_ntDeviceType,
				deviceTypeString,
				deviceInfo->m_connectionCount,
				deviceInfo->m_requestCount
			);

			totalConnectionCount += deviceInfo->m_connectionCount;
			totalRequestCount += deviceInfo->m_requestCount;
		}

		printf(
			"Total devices:    %d\n"
			"Connections:      %d\n"
			"Pending requests: %d\n\n",
			deviceInfoList.getCount(),
			totalConnectionCount,
			totalRequestCount
		);
	}

	return true;
}
#elif (_AXL_OS_LINUX)

bool
printStats(dm::Monitor* monitor) {
	bool result;

	sl::BoxList<dm::HookInfo> hookInfoList;
	result = monitor->getHookInfoList(&hookInfoList);
	if (!result)
		return false;

	size_t totalConnectionCount = 0;

	sl::BoxIterator<dm::HookInfo> it = hookInfoList.getHead();
	for (; it; it++) {
		dm::HookInfo* hookInfo = it.p();

		printf(
			"Module name:      %s\n"
			"Hook file name:   %s\n"
			"Connections:      %d\n\n",
			!hookInfo->m_moduleName.isEmpty() ? hookInfo->m_moduleName.sz() : "<unknown>",
			hookInfo->m_fileName.sz(), // never really empty
			hookInfo->m_connectionCount
		);

		totalConnectionCount += hookInfo->m_connectionCount;
	}

	printf(
		"Total devices:    %d\n"
		"Connections:      %d\n\n",
		hookInfoList.getCount(),
		totalConnectionCount
	);

	return true;
}

#endif

int
printStats() {
	bool result;

	dm::Monitor monitor;
	result = monitor.open();
	if (!result) {
		printf(DM_CORE_DRIVER_FILE_NAMEA " stats are not available: %s\n", err::getLastErrorDescription().sz());
	} else {
		printf(DM_CORE_DRIVER_FILE_NAMEA " stats:\n\n");

		result = printStats(&monitor);
		if (!result)
			printf("Can't query %s: %s\n", DM_MASTER_DEVICE_NAMEA, err::getLastErrorDescription().sz());
	}

#if (_AXL_OS_WIN)
	dm::PnpBeacon beacon;
	result = beacon.open();
	if (!result) {
		printf(DM_PNP_DRIVER_FILE_NAMEA " stats are not available: %s\n\n", err::getLastErrorDescription().sz());
	} else {
		printf(DM_PNP_DRIVER_FILE_NAMEA " stats:\n");

		result = printStats(&beacon);
		if (!result)
			printf("Can't query %s: %s\n", DM_BEACON_DEVICE_NAMEA, err::getLastErrorDescription().sz());
	}
#endif

	return Error_Success;
}

//..............................................................................
