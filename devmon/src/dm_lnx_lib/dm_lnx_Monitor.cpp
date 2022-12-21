#include "pch.h"
#include "dm_lnx_Monitor.h"

namespace dm {

//..............................................................................

bool
Monitor::open() {
	return m_device.open(
		"/dev/" DM_DEVICE_NAME,
		O_RDWR | O_NONBLOCK | O_NOCTTY
	);
}

uint_t
Monitor::getDriverVersion() {
	uint_t version = 0;
	int result = m_device.ioctl(DM_IOCTL_GET_VERSION, &version);
	if (result == -1)
		return -1;

	return version;
}

bool
Monitor::getHookInfoList(sl::BoxList<HookInfo>* resultList) {
	union {
		dm_List m_list;
		char m_buffer[1024];
	} stackBuffer;

	stackBuffer.m_list.m_bufferSize = sizeof(stackBuffer);

	int result = m_device.ioctl(DM_IOCTL_GET_HOOK_INFO_LIST, (void*) &stackBuffer);
	if (result == -1 && errno != ENOBUFS)
		return false;

	sl::Array<char> heapBuffer;
	dm_List* list;

	if (stackBuffer.m_list.m_bufferSize <= sizeof(stackBuffer)) {
		list = &stackBuffer.m_list;
	} else {
		result = heapBuffer.setCount(stackBuffer.m_list.m_bufferSize);
		if (!result)
			return false;

		list = (dm_List*)heapBuffer.p();
		list->m_bufferSize = stackBuffer.m_list.m_bufferSize;

		int result = m_device.ioctl(DM_IOCTL_GET_HOOK_INFO_LIST, heapBuffer.p());
		if (result == -1) {
			if (errno == ENOBUFS)
				err::setError("invalid resposne from '" DM_DRIVER_FILE_NAME "': second-try buffer fail");

			return false;
		}
	}

	dm_HookInfo* hookInfo = (dm_HookInfo*)(list + 1);
	dm_HookInfo* end = (dm_HookInfo*)((char*)hookInfo + list->m_dataSize);

	while (hookInfo < end) {
		ASSERT(hookInfo + 1 <= end);

		const char* moduleName = (const char*) (hookInfo + 1);
		const char* fileName = (const char*) (moduleName + hookInfo->m_moduleNameLength + 1);

		HookInfo resultHookInfo;
		resultHookInfo.m_connectionCount = hookInfo->m_connectionCount;
		resultHookInfo.m_moduleName = sl::String(moduleName, hookInfo->m_moduleNameLength);
		resultHookInfo.m_fileName = sl::String(fileName, hookInfo->m_fileNameLength);
		resultList->insertTail(resultHookInfo);

		hookInfo = (dm_HookInfo*)(fileName + hookInfo->m_fileNameLength + 1);
	}

	return true;
}

bool
Monitor::getTargetHookInfo(HookInfo* resultHookInfo) {
	union {
		dm_HookInfo m_hookInfo;
		char m_buffer[1024];
	} stackBuffer;

	stackBuffer.m_hookInfo.m_bufferSize = sizeof(stackBuffer);

	int result = m_device.ioctl(DM_IOCTL_GET_TARGET_HOOK_INFO, (void*) &stackBuffer);
	if (result == -1 && errno != ENOBUFS)
		return false;

	sl::Array<char> heapBuffer;
	dm_HookInfo* hookInfo;

	if (stackBuffer.m_hookInfo.m_bufferSize <= sizeof(stackBuffer)) {
		hookInfo = &stackBuffer.m_hookInfo;
	} else {
		result = heapBuffer.setCount(stackBuffer.m_hookInfo.m_bufferSize);
		if (!result)
			return false;

		hookInfo = (dm_HookInfo*)heapBuffer.p();
		hookInfo->m_bufferSize = stackBuffer.m_hookInfo.m_bufferSize;

		result = m_device.ioctl(DM_IOCTL_GET_TARGET_HOOK_INFO, heapBuffer.p());

		if (result == -1) {
			if (errno == ENOBUFS)
				err::setError("invalid resposne from '" DM_DRIVER_FILE_NAME "': second-try buffer fail");

			return false;
		}
	}

	const char* moduleName = (const char*) (hookInfo + 1);
	const char* fileName = (const char*) (moduleName + hookInfo->m_moduleNameLength + 1);

	resultHookInfo->m_connectionCount = hookInfo->m_connectionCount;
	resultHookInfo->m_moduleName = sl::String(moduleName, hookInfo->m_moduleNameLength);
	resultHookInfo->m_fileName = sl::String(fileName, hookInfo->m_fileNameLength);

	return true;
}

size_t
Monitor::getPendingNotifySizeLimit() {
	size_t value = 0;
	int result = m_device.ioctl(DM_IOCTL_GET_PENDING_NOTIFY_SIZE_LIMIT, &value);
	return result == 0 ? value : -1;
}

dm_ReadMode
Monitor::getReadMode() {
	union {
		uint_t m_int;
		dm_ReadMode m_readMode;
	} value = { 0 };

	int result = m_device.ioctl(DM_IOCTL_GET_READ_MODE, (void*) &value);
	return result == 0 ? value.m_readMode : dm_ReadMode_Undefined;
}

bool
Monitor::getIoctlDescTable(sl::Array<dm_IoctlDesc>* table) {
	union {
		dm_List m_list;
		char m_buffer[1024];
	} stackBuffer;

	stackBuffer.m_list.m_bufferSize = sizeof(stackBuffer);

	int result = m_device.ioctl(DM_IOCTL_GET_IOCTL_DESC_TABLE, (void*) &stackBuffer);
	if (result == -1 && errno != ENOBUFS)
		return false;

	sl::Array<char> heapBuffer;
	dm_List* list;

	if (stackBuffer.m_list.m_bufferSize <= sizeof(stackBuffer)) {
		list = &stackBuffer.m_list;
	} else {
		result = heapBuffer.setCount(stackBuffer.m_list.m_bufferSize);
		if (!result)
			return false;

		list = (dm_List*)heapBuffer.p();
		list->m_bufferSize = stackBuffer.m_list.m_bufferSize;

		int result = m_device.ioctl(DM_IOCTL_GET_IOCTL_DESC_TABLE, heapBuffer.p());
		if (result == -1) {
			if (errno == ENOBUFS)
				err::setError("invalid resposne from '" DM_DRIVER_FILE_NAME "': second-try buffer fail");

			return false;
		}
	}

	if (list->m_dataSize > list->m_bufferSize || list->m_dataSize != list->m_elementCount * sizeof(dm_IoctlDesc)) {
		err::setError("invalid resposne from '" DM_DRIVER_FILE_NAME "': invalid data size");
		return false;
	}


	table->setCount(list->m_elementCount);
	memcpy(table->p(), list + 1, list->m_dataSize);
	return true;
}

bool
Monitor::setIoctlDescTable(
	const dm_IoctlDesc* ioctlDesc,
	size_t count
) {
	char stackBuffer[256];
	sl::Array<char> buffer(rc::BufKind_Stack, stackBuffer, sizeof(stackBuffer));

	size_t size = count * sizeof(dm_IoctlDesc);

	bool result = buffer.setCount(sizeof(dm_List) + size);
	if (!result)
		return false;

	dm_List* table = (dm_List*)buffer.p();
	table->m_elementCount = count;
	table->m_dataSize = size;
	memcpy(table + 1, ioctlDesc, size);

	return m_device.ioctl(DM_IOCTL_SET_IOCTL_DESC_TABLE, table) == 0;
}

size_t
Monitor::read(
	void* p,
	size_t size,
	uint_t timeout
) {
	if (!isOpen()) {
		err::setError(err::SystemErrorCode_InvalidDeviceState);
		return -1;
	}

	size_t result;

	if (timeout == 0) { // shortcut
		result = m_device.read(p, size);
		return result != -1 ?
			result :
			errno == EWOULDBLOCK ? 0 : -1;
	}

	fd_set readSet = { 0 };
	FD_SET(m_device, &readSet);

	if (timeout == -1) {
		result = ::select(m_device + 1, &readSet, NULL, NULL, NULL);
	} else {
		timeval tv;
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;

		result = ::select(m_device + 1, &readSet, NULL, NULL, &tv);
		if (result == 0)
			return 0;
	}

	if (result == -1) {
		err::setLastSystemError();
		return -1;
	}

	return m_device.read(p, size);
}

bool
Monitor::verifyMessageBuffer(
	const void* buffer,
	size_t bufferSize,
	size_t actualSize
) {
	ASSERT(bufferSize >= sizeof(dm_NotifyHdr));

	dm_NotifyHdr* notifyHdr = (dm_NotifyHdr*)buffer;
	if (actualSize < sizeof(dm_NotifyHdr)) {
		err::setFormatStringError(
			"invalid resposne from '" DM_DRIVER_FILE_NAME "': "
			"buffer too small (%d; expected: %d)",
			actualSize,
			sizeof(dm_NotifyHdr)
		);

		return false;
	}

	if (notifyHdr->m_signature != dm_NotifyHdrSignature) {
		err::setFormatStringError(
			"invalid resposne from '" DM_DRIVER_FILE_NAME "': "
			"bad signature (0x%08x; expected: 0x%08x)",
			notifyHdr->m_signature,
			dm_NotifyHdrSignature
		);

		return false;
	}

	if (notifyHdr->m_flags & dm_NotifyFlag_InsufficientBuffer) {
		size_t newBufferSize = sizeof(dm_NotifyHdr) + notifyHdr->m_paramSize;
		if (newBufferSize <= bufferSize) {
			err::setFormatStringError(
				"invalid resposne from '" DM_DRIVER_FILE_NAME "': "
				"false-report of insufficient buffer (%d; supplied: %d)",
				newBufferSize,
				bufferSize
			);

			return false;
		}
	} else if (sizeof(dm_NotifyHdr) + notifyHdr->m_paramSize > actualSize) {
		err::setFormatStringError(
			"invalid resposne from '" DM_DRIVER_FILE_NAME "': "
			"invalid params size (%d; delivered: %d)",
			notifyHdr->m_paramSize,
			actualSize - sizeof(dm_NotifyHdr)
		);

		return false;
	}

	return true;
}

bool
Monitor::getBoolImpl(uint_t ioctlCode) {
	uint_t value = false;
	int result = m_device.ioctl(ioctlCode, &value);
	return result == 0 ? value != 0 : false;
}

sl::String
Monitor::getStringImpl(uint_t ioctlCode) {
	union {
		dm_String m_string;
		char m_buffer[1024];
	} buffer;

	buffer.m_string.m_bufferSize = sizeof(buffer);

	int result = m_device.ioctl(ioctlCode, (void*) &buffer);
	if (result == -1)
		sl::String();

	const char* p = (const char*) (&buffer.m_string + 1);
	return sl::String(p, buffer.m_string.m_length);
}

bool
Monitor::stringIoctlImpl(
	uint_t ioctlCode,
	const sl::String& string
) {
	size_t length = string.getLength();
	size_t fullSize = sizeof(dm_String) + length + 1;

	char stackBuffer[256];
	sl::Array<char> buffer(rc::BufKind_Stack, stackBuffer, sizeof(stackBuffer));
	bool result = buffer.setCount(fullSize);
	if (!result)
		return false;

	dm_String* param = (dm_String*)buffer.p();
	param->m_bufferSize = fullSize;
	param->m_length = length;
	memcpy(param + 1, string.cp(), length); // sl::Array is zero-initialized

	return m_device.ioctl(ioctlCode, param) == 0;
}

//..............................................................................

} // namespace dm
