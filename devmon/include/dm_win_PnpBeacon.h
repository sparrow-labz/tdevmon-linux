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

#include "dm_win_DeviceMgr.h"
#include "dm_win_Protocol.h"

namespace dm {

//..............................................................................

class PnpBeacon: public DeviceMgr {
public:
	PnpBeacon():
		DeviceMgr(DeviceMgrKind_PnpBeacon) {}
};

//..............................................................................

} // namespace dm
