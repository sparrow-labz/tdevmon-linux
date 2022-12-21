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

enum MonitorDef {
	MonitorDef_InitialParamSize = 3 * 1024, // 1st try with 3K (so one page is enough)
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class Monitor: public DeviceMgr {
public:
	Monitor():
		DeviceMgr(DeviceMgrKind_Monitor) {}

	bool
	checkAccess() {
		return simpleIoctlImpl(DM_IOCTL_CHECK_ACCESS);
	}

	bool
	getTargetDeviceInfo(DeviceInfo* deviceInfo);

	bool
	stop() {
		return simpleIoctlImpl(DM_IOCTL_STOP);
	}

	bool
	isConnected() {
		return getBoolImpl(DM_IOCTL_IS_CONNECTED);
	}

	bool
	connect(const axl::sl::StringRef& fileName);

	bool
	disconnect() {
		return simpleIoctlImpl(DM_IOCTL_DISCONNECT);
	}

	bool
	isEnabled() {
		return getBoolImpl(DM_IOCTL_IS_ENABLED);
	}

	bool
	enable() {
		return simpleIoctlImpl(DM_IOCTL_ENABLE);
	}

	bool
	disable() {
		return simpleIoctlImpl(DM_IOCTL_DISABLE);
	}

	size_t
	getPendingNotifySizeLimit();

	bool
	setPendingNotifySizeLimit(size_t limit);

	dm_ReadMode
	getReadMode();

	bool
	setReadMode(dm_ReadMode mode);

	sl::String
	getFileNameFilter();

	bool
	setFileNameFilter(const sl::StringRef& filter);

	size_t
	read(
		void* p,
		size_t size
	) {
		return m_device.overlappedRead(p, size);
	}

	bool
	overlappedRead(
		void* p,
		dword_t size,
		OVERLAPPED* overlapped
	) {
		return m_device.overlappedRead(p, size, overlapped);
	}

	bool
	getOverlappedResult(
		OVERLAPPED* overlapped,
		dword_t* actualSize
	) {
		return m_device.getOverlappedResult(overlapped, actualSize);
	}

	size_t
	getOverlappedResult(OVERLAPPED* overlapped) {
		return m_device.getOverlappedResult(overlapped);
	}

#ifdef _AXL_DEBUG
	bool
	test();
#endif

	static
	bool
	verifyMessageBuffer(
		const void* buffer,
		size_t bufferSize,
		size_t actualSize
	);
};

//..............................................................................

} // namespace dm
