#pragma once

#include "dm_win_Service.h"

//..............................................................................

int
createService(dm::ServiceKind serviceKind);

int
deleteService(dm::ServiceKind serviceKind);

int
startCoreService();

int
stopCoreService(bool isForced);

int
restartCoreService(bool isForced);

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

int
install();

int
uninstall(bool isForced);

int
reinstall(bool isForced);

//..............................................................................
