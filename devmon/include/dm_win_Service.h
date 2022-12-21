//..............................................................................
//
//  This file is part of Tibbo Device Monitor.
//  Copyright (c) 2014-2017, Tibbo Technology Inc
//  Author: Vladimir Gladkov
//
//  For legal details see accompanying license.txt file,
//  the public copy of which is also available at:
//  http://tibbo.com/downloads/archive/tdevmon/license.txt
//
//..............................................................................

#pragma once

#include "dm_win_Protocol.h"

namespace dm {

//..............................................................................

enum ServiceKind {
	ServiceKind_Core,
	ServiceKind_Pnp,
	ServiceKind__Count
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

const wchar_t*
getServiceName(ServiceKind serviceKind);

bool
isServiceCreated(ServiceKind serviceKind);

bool
createService(ServiceKind serviceKind);

bool
deleteService(ServiceKind serviceKind);

bool
isCoreServiceStarted();

bool
startCoreService();

bool
stopCoreService();

bool
forceStopCoreService(bool* isRebootRequired);

bool
isPnpFilterInstalled(axl::sys::win::DeviceInfo* deviceInfo);

bool
installPnpFilter(
	axl::sys::win::DeviceInfo* deviceInfo,
	bool* isRebootRequired
);

bool
uninstallPnpFilter(
	axl::sys::win::DeviceInfo* deviceInfo,
	bool* isRebootRequired
);

bool
openCoreServiceParameterRegKey(
	HKEY* hkey,
	REGSAM access
);

//..............................................................................

} // namespace dm
