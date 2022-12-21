#pragma once

#if (_AXL_OS_WIN)
#	include "dm_win_Protocol.h"
#	include "dm_win_Service.h"
#elif (_AXL_OS_LINUX)
#	include "dm_lnx_Protocol.h"
#endif

//..............................................................................

enum CmdLineVerb {
	CmdLineVerb_Help,
	CmdLineVerb_Version,
	CmdLineVerb_Stats,
	CmdLineVerb_Monitor,
#if (_AXL_OS_WIN)
	CmdLineVerb_Install,
	CmdLineVerb_Uninstall,
	CmdLineVerb_Reinstall,
	CmdLineVerb_CreateService,
	CmdLineVerb_DeleteService,
	CmdLineVerb_StartCoreService,
	CmdLineVerb_StopCoreService,
	CmdLineVerb_RestartCoreService,
	CmdLineVerb_AddPnpFilter,
	CmdLineVerb_RemovePnpFilter,
	CmdLineVerb_RemoveAllPnpFilters,
	CmdLineVerb_ListClassDevices,
	CmdLineVerb_ListAllDevices,
	CmdLineVerb_DeviceInfo,
	CmdLineVerb_CheckAccess,
	CmdLineVerb_SetAccess,
	CmdLineVerb_ShowSd,
	CmdLineVerb_AccountInfo,
	CmdLineVerb_ListAllAccounts,
#	ifdef _AXL_DEBUG
	CmdLineVerb_Test = 0x0200,
#	endif
#elif (_AXL_OS_LINUX)
	CmdLineVerb_Unhook,
	CmdLineVerb_Stop,
#endif
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

enum CmdLineFlag {
#if (_AXL_OS_WIN)
	CmdLineFlag_Force         = 0x01,
	CmdLineFlag_AdminRequired = 0x02,
	CmdLineFlag_ClearSd       = 0x04,
	CmdLineFlag_SetDefaultSd  = 0x08,
#endif
	CmdLineFlag_MachineIface  = 0x10,
};
//..............................................................................

struct CmdLine {
	CmdLineVerb m_verb;
	uint_t m_flags;
	sl::String m_deviceName;
	sl::String m_fileNameWildcard;
#if (_AXL_OS_WIN)
	sl::String m_deviceClassName;
	sl::String m_accountName;
	dm::ServiceKind m_serviceKind;
	sl::BoxList<sl::String> m_denyList;
	sl::BoxList<sl::String> m_allowList;
#elif (_AXL_OS_LINUX)
	sl::Array<dm_IoctlDesc> m_ioctlDescArray;
#endif
	CmdLine();
};

//..............................................................................

enum CmdLineSwitch {
	CmdLineSwitch_None,
	CmdLineSwitch_Help,
	CmdLineSwitch_Version,
	CmdLineSwitch_Stats,
	CmdLineSwitch_Monitor,
	CmdLineSwitch_FileNameWildcard,
#if (_AXL_OS_WIN)
	CmdLineSwitch_Install,
	CmdLineSwitch_Uninstall,
	CmdLineSwitch_Reinstall,
	CmdLineSwitch_CreateCoreService,
	CmdLineSwitch_DeleteCoreService,
	CmdLineSwitch_StartCoreService,
	CmdLineSwitch_StopCoreService,
	CmdLineSwitch_RestartCoreService,
	CmdLineSwitch_CreatePnpService,
	CmdLineSwitch_DeletePnpService,
	CmdLineSwitch_AddPnpFilter,
	CmdLineSwitch_RemovePnpFilter,
	CmdLineSwitch_RemoveAllPnpFilters,
	CmdLineSwitch_Force,
	CmdLineSwitch_ListClassDevices,
	CmdLineSwitch_ListAllDevices,
	CmdLineSwitch_DeviceInfo,
	CmdLineSwitch_CheckAccess,
	CmdLineSwitch_ShowSd,
	CmdLineSwitch_ClearSd,
	CmdLineSwitch_SetDefaultSd,
	CmdLineSwitch_AddAllowAce,
	CmdLineSwitch_AddDenyAce,
	CmdLineSwitch_AccountInfo,
	CmdLineSwitch_ListAllAccounts,
#elif (_AXL_OS_LINUX)
	CmdLineSwitch_Unhook,
	CmdLineSwitch_Stop,
	CmdLineSwitch_IoctlDesc,
#endif
	CmdLineSwitch_MachineIface,

#ifdef _AXL_DEBUG
	CmdLineSwitch_Test,
#endif
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

AXL_SL_BEGIN_CMD_LINE_SWITCH_TABLE(CmdLineSwitchTable, CmdLineSwitch)
	AXL_SL_CMD_LINE_SWITCH_GROUP("General")
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_Help,
		"h", "help", NULL,
		"Display this help"
	)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_Version,
		"v", "version", NULL,
		"Display version information"
	)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_Stats,
		"s", "stats", NULL,
		"Display statistics of kernel-mode drivers"
	)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_Monitor,
		"m", "monitor", "<device>",
		"Connect and monitor activity on specified device"
	)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_FileNameWildcard,
		"n", "filename", "<wildcard>",
		"Use filename wildcard filter"
	)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_MachineIface,
		"mi", "machine-iface", NULL,
		"When monitoring, output to STDOUT in binary format"
	)

#if (_AXL_OS_WIN)
	AXL_SL_CMD_LINE_SWITCH_GROUP("Device Information")
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_ListClassDevices,
		"list-class-devices", "list-class-devs", "<class>",
		"Display list of devices of specified class"
	)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_ListAllDevices,
		"list-all-devices", "list-all-devs", NULL,
		"Display list of all devices"
	)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_DeviceInfo,
		"device-info", "dev-info", "<device>",
		"Display information about specified device"
	)

	AXL_SL_CMD_LINE_SWITCH_GROUP("Service Management")
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_Install,
		"i", "install", NULL,
		"Create and start services"
	)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_Uninstall,
		"u", "uninstall", NULL,
		"Stop and delete services"
	)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_Reinstall,
		"r", "reinstall", NULL,
		"Create (if necessary) and restart services"
	)
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_CreateCoreService,
		"create-core-service", NULL,
		"Create kernel-mode service " DM_CORE_SERVICE_NAMEA
	)
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_DeleteCoreService,
		"delete-core-service", NULL,
		"Delete kernel-mode service " DM_CORE_SERVICE_NAMEA
	)
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_StartCoreService,
		"start-core-service", NULL,
		"Start kernel-mode service " DM_CORE_SERVICE_NAMEA
	)
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_StopCoreService,
		"stop-core-service", NULL,
		"Stop kernel-mode service " DM_CORE_SERVICE_NAMEA
	)
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_RestartCoreService,
		"restart-core-service", NULL,
		"Restart kernel-mode service " DM_CORE_SERVICE_NAMEA
	)
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_CreatePnpService,
		"create-pnp-service", NULL,
		"Create kernel-mode service " DM_PNP_SERVICE_NAMEA
	)
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_DeletePnpService,
		"delete-pnp-service", NULL,
		"Delete kernel-mode service " DM_PNP_SERVICE_NAMEA
	)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_Force,
		"f", "force", NULL,
		"Force service stop (when in use)"
	)

	AXL_SL_CMD_LINE_SWITCH_GROUP("PNP Filter Management")
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_AddPnpFilter,
		"add-pnp-filter", "<device-or-class>",
		"Add PNP filter " DM_PNP_SERVICE_NAMEA " for specified device(s)"
	)
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_RemovePnpFilter,
		"remove-pnp-filter", "<device-or-class>",
		"Remove PNP filter " DM_PNP_SERVICE_NAMEA " for specified device(s)"
	)
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_RemoveAllPnpFilters,
		"remove-all-pnp-filters", NULL,
		"Remove PNP filter " DM_PNP_SERVICE_NAMEA " for all devices"
	)

	AXL_SL_CMD_LINE_SWITCH_GROUP("Security Management")
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_CheckAccess,
		"check-access", NULL,
		"Check whether the current user is allowed to use monitor"
	)

	AXL_SL_CMD_LINE_SWITCH_3(
		CmdLineSwitch_AddAllowAce,
		"a", "allow", "add-allow-ace", "<account>",
		"Allow user or group to use monitor"
	)

	AXL_SL_CMD_LINE_SWITCH_3(
		CmdLineSwitch_AddDenyAce,
		"d", "deny", "add-deny-ace", "<account>",
		"Deny user or group to use monitor"
	)

	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_ShowSd,
		"show-sd", NULL,
		"Display security descriptor"
	)

	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_ClearSd,
		"clear-sd", NULL,
		"Clear security descriptor before adding new ACEs"
	)

	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_SetDefaultSd,
		"set-default-sd", NULL,
		"Set default security descriptor"
	)

	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_AccountInfo,
		"account-info", "<account>",
		"Display user or group account information"
	)

	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_ListAllAccounts,
		"list-all-accounts", NULL,
		"List all users and group accounts on this machine"
	)
#	if (_AXL_DEBUG)
	AXL_SL_CMD_LINE_SWITCH_GROUP("Debug")
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_Test,
		"test", NULL,
		"Issue DM_IOCTL_TEST to \\Device\\" DM_MASTER_DEVICE_NAMEA
	)
	#endif
#elif (_AXL_OS_LINUX)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_Unhook,
		"u", "unhook", "<device>",
		"Attempt to remove hooks from the specified device"
	)
	AXL_SL_CMD_LINE_SWITCH_2(
		CmdLineSwitch_Stop,
		"x", "stop", NULL,
		"Attempt to remove all hooks and stop kernel module"
	)
	AXL_SL_CMD_LINE_SWITCH(
		CmdLineSwitch_IoctlDesc,
		"ioctl", "<code:arg-size>",
		"Specify ioctl descriptor (multiple allowed)"
	)
#endif
AXL_SL_END_CMD_LINE_SWITCH_TABLE()

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CmdLineParser: public sl::CmdLineParser<CmdLineParser, CmdLineSwitchTable> {
	friend class sl::CmdLineParser<CmdLineParser, CmdLineSwitchTable>;

protected:
	CmdLine* m_cmdLine;

public:
	CmdLineParser(CmdLine* cmdLine) {
		m_cmdLine = cmdLine;
	}

protected:
	bool
	onValue(const sl::StringRef& value);

	bool
	onSwitch(
		SwitchKind switchKind,
		const sl::StringRef& value
	);

#if (_AXL_OS_LINUX)
	static
	bool
	parseIoctlDesc(
		dm_IoctlDesc* ioctlDesc,
		const sl::StringRef& string
	);
#endif
};

//..............................................................................
