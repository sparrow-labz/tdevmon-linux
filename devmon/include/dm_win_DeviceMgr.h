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

namespace dm {

//..............................................................................

struct DeviceInfo {
	uint_t m_flags;
	uint_t m_ntDeviceType;
	size_t m_connectionCount;
	size_t m_requestCount;
	axl::sl::String m_deviceName;
};

//..............................................................................

enum DeviceMgrKind {
	DeviceMgrKind_Undefined,
	DeviceMgrKind_Monitor,
	DeviceMgrKind_PnpBeacon,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class DeviceMgr {
protected:
	axl::io::win::File m_device;
	DeviceMgrKind m_deviceMgrKind;

protected:
	DeviceMgr(DeviceMgrKind deviceMgrKind) {
		m_deviceMgrKind = deviceMgrKind;
	}

public:
	DeviceMgrKind
	getDeviceMgrKind() {
		return m_deviceMgrKind;
	}

	bool
	isOpen() {
		return m_device.isOpen();
	}

	bool
	open();

	void
	close() {
		m_device.close();
	}

	uint_t
	getDriverVersion();

	axl::sl::String
	getDriverDescription();

	axl::sl::String
	getDriverBuildTime();

	bool
	getDeviceInfoList(axl::sl::BoxList<DeviceInfo>* deviceInfoList);

protected:
	bool
	simpleIoctlImpl(uint_t ioctlCode);

	bool
	getBoolImpl(uint_t ioctlCode);

	bool
	getBlobImpl(
		uint_t ioctlCode,
		sl::Array<char>* buffer
	);
};

//..............................................................................

} // namespace dm
