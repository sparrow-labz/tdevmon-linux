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

#include "dm_lnx_Protocol.h"

namespace dm {

//..............................................................................

enum MonitorDef {
	MonitorDef_InitialParamsSize = 3 * 1024, // 1st try with 3K (so one page is enough)
};

struct HookInfo {
	size_t m_connectionCount;
	axl::sl::String m_moduleName;
	axl::sl::String m_fileName;
};

//..............................................................................

class Monitor {
public:
	axl::io::psx::File m_device; // can be selected/polled

public:
	Monitor() {}

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
	getDriverDescription() {
		return getStringImpl(DM_IOCTL_GET_DESCRIPTION);
	}

	axl::sl::String
	getDriverBuildTime() {
		return getStringImpl(DM_IOCTL_GET_BUILD_TIME);
	}

	bool
	getHookInfoList(axl::sl::BoxList<HookInfo>* list);

	bool
	getTargetHookInfo(HookInfo* hookInfo);

	bool
	isConnected() {
		return getBoolImpl(DM_IOCTL_IS_CONNECTED);
	}

	bool
	connect(const axl::sl::StringRef& fileName) {
		return stringIoctlImpl(DM_IOCTL_CONNECT, fileName);
	}

	bool
	disconnect() {
		return m_device.ioctl(DM_IOCTL_DISCONNECT) == 0;
	}

	bool
	isEnabled() {
		return getBoolImpl(DM_IOCTL_IS_ENABLED);
	}

	bool
	enable() {
		return m_device.ioctl(DM_IOCTL_ENABLE) == 0;
	}

	bool
	disable() {
		return m_device.ioctl(DM_IOCTL_DISABLE) == 0;
	}

	size_t
	getPendingNotifySizeLimit();

	bool
	setPendingNotifySizeLimit(size_t limit) {
		return m_device.ioctl(DM_IOCTL_SET_PENDING_NOTIFY_SIZE_LIMIT, limit) == 0;
	}

	dm_ReadMode
	getReadMode();

	bool
	setReadMode(dm_ReadMode mode) {
		return m_device.ioctl(DM_IOCTL_SET_READ_MODE, mode) == 0;
	}

	sl::String
	getFileNameFilter() {
		return getStringImpl(DM_IOCTL_GET_FILE_NAME_FILTER);
	}

	bool
	setFileNameFilter(const sl::StringRef& filter) {
		return stringIoctlImpl(DM_IOCTL_SET_FILE_NAME_FILTER, filter);
	}

	bool
	getIoctlDescTable(sl::Array<dm_IoctlDesc>* table);

	sl::Array<dm_IoctlDesc>
	getIoctlDescTable() {
		sl::Array<dm_IoctlDesc> table;
		getIoctlDescTable(&table);
		return table;
	}

	bool
	setIoctlDescTable(
		const dm_IoctlDesc* ioctlDesc,
		size_t count
	);

	bool
	unhook(const axl::sl::StringRef& fileName) {
		return stringIoctlImpl(DM_IOCTL_UNHOOK, fileName);
	}

	bool
	stop() {
		return m_device.ioctl(DM_IOCTL_STOP) == 0;
	}

	size_t
	read(
		void* p,
		size_t size,
		uint_t timeout = -1
	);

	static
	bool
	verifyMessageBuffer(
		const void* buffer,
		size_t bufferSize,
		size_t actualSize
	);

protected:
	bool
	getBoolImpl(uint_t ioctlCode);

	sl::String
	getStringImpl(uint_t ioctlCode);

	bool
	stringIoctlImpl(
		uint_t ioctlCode,
		const sl::String& string
	);
};

//..............................................................................

} // namespace dm
