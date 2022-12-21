#include "pch.h"
#include "monitor.h"
#include "error.h"
#include "dm_MachineIface.h"

#if (_AXL_OS_WIN)
#	include "dm_win_Monitor.h"
#elif (_AXL_OS_LINUX)
#	include "dm_lnx_Monitor.h"
#endif

//..............................................................................

class StdinThread: public sys::ThreadImpl<StdinThread> {
public:
	void
	threadFunc();
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void
StdinThread::threadFunc() {
#if (_AXL_OS_WIN)
	HANDLE h = ::GetStdHandle(STD_INPUT_HANDLE);
#endif

	for (;;) {
		char buffer[64];

#if (_AXL_OS_WIN)
		dword_t size;
		bool_t result = ::ReadFile(h, buffer, sizeof(buffer), &size, NULL);
		if (!result || size == 0)
			break;
#elif (_AXL_OS_POSIX)
		int result = ::read(STDIN_FILENO, buffer, sizeof(buffer));
		if (result == 0 || result == -1)
			break;
#endif
	}

#if (_AXL_OS_WIN)
	::GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
#elif (_AXL_OS_POSIX)
	::raise(SIGINT);
#else
#	error unsupported OS
#endif
}

//..............................................................................

void
printHexLine(
	size_t offset,
	const void* data,
	size_t size
) {
	const uchar_t* p = (const uchar_t*) data;

	printf("    %04x  ", offset & 0xffff);

	size_t i;
	for (i = 0; i < size; i++)
		printf("%02x ", p [i]);

	printf(" ");

	for (; i < 16; i++)
		printf("   ");

	for (i = 0; i < size; i++) {
		uchar_t c = p[i];
		printf("%c", isprint (c) ? c : '.');
	}

	printf("\n");
}

void
printHexView(
	const void* data,
	size_t size
) {
	const uchar_t* p = (const uchar_t*) data;
	const uchar_t* p0 = p;
	const uchar_t* end1 = p + (size & ~0xf);
	const uchar_t* end2 = p + size;

	while (p < end1) {
		printHexLine(p - p0, p, 16);
		p += 16;
	}

	if (p < end2)
		printHexLine(p - p0, p, end2 - p);
}

void
print_mi(
	uint_t code,
	const void* p,
	size_t size
) {
	dm_MiMsgHdr hdr;
	hdr.m_code = code;
	hdr.m_paramSize = size;

	fwrite(&hdr, sizeof(hdr), 1, stdout);
	fwrite(p, size, 1, stdout);
}

void
print_mi(
	uint_t code,
	const void* p1,
	size_t size1,
	const void* p2,
	size_t size2
) {
	dm_MiMsgHdr hdr;
	hdr.m_code = code;
	hdr.m_paramSize = size1 + size2;

	fwrite(&hdr, sizeof(hdr), 1, stdout);
	fwrite(p1, size1, 1, stdout);
	fwrite(p2, size2, 1, stdout);
}

inline
void
print_mi(uint_t code) {
	return print_mi(code, NULL, 0);
}

inline
void
printError_mi(const err::ErrorRef& error) {
	return print_mi(dm_MiMsgCode_Error, error, error->m_size);
}

inline
void
printLastError_mi() {
	return printError_mi(err::getLastError());
}

#if (_AXL_OS_WIN)

int
monitorDevice(
	const sl::StringRef& deviceName,
	const sl::StringRef& fileNameWildcard
) {
	static const char* notifyCodeStringTable[dm_NotifyCode__Count] = {
		"<UNKNOWN>",                        // dm_NotifyCode_Undefined,
		"IRP_MJ_PNP::IRP_MN_START_DEVICE",  // dm_NotifyCode_PnpStartDevice,
		"IRP_MJ_PNP::IRP_MN_STOP_DEVICE",   // dm_NotifyCode_PnpStopDevice,
		"IRP_MJ_PNP::IRP_MN_REMOVE_DEVICE", // dm_NotifyCode_PnpRemoveDevice,
		"FastIoDetachDevice",               // dm_NotifyCode_FastIoDetachDevice,
		"IRP_MJ_CREATE",                    // dm_NotifyCode_Create,
		"IRP_MJ_CREATE_NAMED_PIPE",         // dm_NotifyCode_CreateNamedPipe,
		"IRP_MJ_CLOSE",                     // dm_NotifyCode_Close,
		"IRP_MJ_READ",                      // dm_NotifyCode_Read,
		"IRP_MJ_WRITE",                     // dm_NotifyCode_Write,
		"IRP_MJ_DEVICE_CONTROL",            // dm_NotifyCode_Ioctl,
		"IRP_MJ_INTERNAL_DEVICE_CONTROL",   // dm_NotifyCode_InternalIoctl,
		"IRP_MJ_FILE_SYSTEM_CONTROL",       // dm_NotifyCode_Fsctl,
		"IRP_MJ_CREATE_MAILSLOT",           // dm_NotifyCode_CreateMailslot,
		"KeyboardEvent",                    // dm_NotifyCode_KeyboardEvent,
		"MouseEvent",                       // dm_NotifyCode_MouseEvent,
		"FastIoRead",	                    // dm_NotifyCode_FastIoRead,
		"FastIoWrite",	                    // dm_NotifyCode_FastIoWrite,
		"FastIoDeviceControl",	            // dm_NotifyCode_FastIoIoctl,
	};

	bool result;

	setvbuf(stdout, NULL, _IONBF, 0); // turn off buffering of stdout

	dm::Monitor monitor;
	result = monitor.open();
	if (!result) {
		printf("Can't open %s: %s\n", DM_MASTER_DEVICE_NAMEA, err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	uint_t version = monitor.getDriverVersion();
	if (version < 0x030300) {
		printf(
			"Incompatible driver version (reported: %d.%d.%d; required: %d.%d.xx)\n",
			LOBYTE(HIWORD(version)),
			HIBYTE(LOWORD(version)),
			LOBYTE(LOWORD(version)),
			3,
			3
		);

		return Error_InvalidVersion;
	}

	result = monitor.connect(deviceName);
	if (!result) {
		printf("Can't connect to %s: %s\n", deviceName.sz(), err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	dm::DeviceInfo deviceInfo;

	result =
		monitor.getTargetDeviceInfo(&deviceInfo) &&
		monitor.setReadMode(dm_ReadMode_Message) &&
		monitor.setFileNameFilter(fileNameWildcard) &&
		monitor.enable();

	if (!result) {
		printf("Can't configure monitor on %s: %s\n", deviceName.sz(), err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	if (!fileNameWildcard.isEmpty())
		printf("Monitoring %s\\%s", deviceInfo.m_deviceName.sz(), fileNameWildcard.sz());
	else
		printf("Monitoring %s", deviceInfo.m_deviceName.sz());

	printf(", hit Ctrl+C to interrupt...\n");

	sl::Array<char> buffer;
	buffer.setCount(1024); // reserve 1K

	for (;;) {
		size_t actualSize = monitor.read(buffer, buffer.getCount());
		if (actualSize == -1) {
			printf("Read error: %s\n", err::getLastErrorDescription().sz());
			return Error_IoFailure;
		}

		dm_NotifyHdr* notifyHdr = (dm_NotifyHdr*)buffer.p();
		if (notifyHdr->m_flags & dm_NotifyFlag_InsufficientBuffer) {
			size_t bufferSize = sizeof(dm_NotifyHdr) + notifyHdr->m_paramSize;
			result = buffer.setCount(bufferSize);
			if (!result) {
				printf("Read error: %s\n", err::getLastErrorDescription().sz());
				return false;
			}

			actualSize = monitor.read(buffer, bufferSize);
			if (actualSize == -1) {
				printf("Read error: %s\n", err::getLastErrorDescription().sz());
				return Error_IoFailure;
			}
		}

		size_t i = notifyHdr->m_code < countof(notifyCodeStringTable) ? notifyHdr->m_code : 0;
		sl::String timestampString = sys::Time(notifyHdr->m_timestamp).format("%h:%m:%s.%l");

		printf(
			"\n"
			"%s %s\n"
			"    process ID:      %08x\n"
			"    thread ID:       %08x\n"
			"    status:          %08x\n",
			timestampString.sz(),
			notifyCodeStringTable[i],
			notifyHdr->m_processId,
			notifyHdr->m_threadId,
			notifyHdr->m_ntStatus
		);

		dm_NotifyParamsPtr params = { notifyHdr + 1 };

		switch (notifyHdr->m_code) {
		case dm_NotifyCode_PnpStartDevice:
		case dm_NotifyCode_PnpStopDevice:
		case dm_NotifyCode_PnpRemoveDevice:
			break;

		case dm_NotifyCode_Close:
			printf(
				"    file id:         %016llx\n",
				params.m_closeParams->m_fileId64
			);
			break;

		case dm_NotifyCode_Create:
			printf(
				"    file id:         %016llx\n"
				"    options:         %08x\n"
				"    desired access:  %08x\n"
				"    share access:    %08x\n"
				"    file attributes: %08x\n"
				"    file name:       %S\n",
				params.m_createParams->m_fileId64,
				params.m_createParams->m_options,
				params.m_createParams->m_desiredAccess,
				params.m_createParams->m_shareAccess,
				params.m_createParams->m_fileAttributes,
				params.m_createParams + 1
			);

			break;

		case dm_NotifyCode_CreateNamedPipe:
			printf(
				"    file id:         %016llx\n"
				"    options:         %08x\n"
				"    desired access:  %08x\n"
				"    share access:    %08x\n"
				"    pipe type:       %d\n"
				"    read mode:       %d\n"
				"    completion mode: %d\n"
				"    max instances:   %d\n"
				"    inbound quota:   %d\n"
				"    outbound quota:  %d\n"
				"    default timeout: %lld\n"
				"    file name:       %S\n",
				params.m_createNamedPipeParams->m_fileId64,
				params.m_createNamedPipeParams->m_options,
				params.m_createNamedPipeParams->m_desiredAccess,
				params.m_createNamedPipeParams->m_shareAccess,
				params.m_createNamedPipeParams->m_pipeType,
				params.m_createNamedPipeParams->m_readMode,
				params.m_createNamedPipeParams->m_completionMode,
				params.m_createNamedPipeParams->m_maxInstanceCount,
				params.m_createNamedPipeParams->m_inBoundQuota,
				params.m_createNamedPipeParams->m_outBoundQuota,
				params.m_createNamedPipeParams->m_defaultTimeout,
				params.m_createNamedPipeParams + 1
			);

			break;

		case dm_NotifyCode_CreateMailslot:
			printf(
				"    file id:         %016llx\n"
				"    options:         %08x\n"
				"    desired access:  %08x\n"
				"    share access:    %08x\n"
				"    mailslot quota:  %d\n"
				"    max message:     %d\n"
				"    read timeout:    %lld\n"
				"    file name:       %S\n",
				params.m_createMailslotParams->m_fileId64,
				params.m_createMailslotParams->m_options,
				params.m_createMailslotParams->m_desiredAccess,
				params.m_createMailslotParams->m_shareAccess,
				params.m_createMailslotParams->m_mailslotQuota,
				params.m_createMailslotParams->m_maxMessageSize,
				params.m_createMailslotParams->m_readTimeout,
				params.m_createMailslotParams + 1
			);

			break;

		case dm_NotifyCode_Read:
		case dm_NotifyCode_Write:
		case dm_NotifyCode_FastIoRead:
		case dm_NotifyCode_FastIoWrite:
			printf(
				"    file id:         %016llx\n"
				"    offset:          %lld\n"
				"    buffer size:     %d\n"
				"    data size:       %d\n",
				params.m_readWriteParams->m_fileId64,
				params.m_readWriteParams->m_offset,
				params.m_readWriteParams->m_bufferSize,
				params.m_readWriteParams->m_dataSize
			);

			if (params.m_readWriteParams->m_dataSize) {
				printf("\n");
				printHexView(
					params.m_readWriteParams + 1,
					params.m_readWriteParams->m_dataSize
				);
			}

			break;

		case dm_NotifyCode_Ioctl:
		case dm_NotifyCode_InternalIoctl:
		case dm_NotifyCode_Fsctl:
		case dm_NotifyCode_FastIoIoctl:
			printf(
				"    file id:         %016llx\n"
				"    code:            %08x\n"
				"    in data size:    %d\n"
				"    out buffer size: %d\n"
				"    out data size:   %d\n",
				params.m_ioctlParams->m_fileId64,
				params.m_ioctlParams->m_code,
				params.m_ioctlParams->m_inDataSize,
				params.m_ioctlParams->m_outBufferSize,
				params.m_ioctlParams->m_outDataSize
			);

			if (params.m_ioctlParams->m_inDataSize) {
				printf("\n");
				printHexView(
					params.m_ioctlParams + 1,
					params.m_ioctlParams->m_inDataSize
				);
			}

			if (params.m_ioctlParams->m_outDataSize) {
				printf("\n");
				printHexView(
					(char*)(params.m_ioctlParams + 1) + params.m_ioctlParams->m_inDataSize,
					params.m_ioctlParams->m_outDataSize
				);
			}

			break;

		case dm_NotifyCode_KeyboardEvent:
			printf(
				"    input count:     %d\n",
				params.m_keyboardEventParams->m_inputCount
			);

			for (size_t i = 0; i < params.m_keyboardEventParams->m_inputCount; i++) {
				const dm_KeyboardInput* input = (const dm_KeyboardInput*) (params.m_mouseEventParams + 1) + i;
				printf("    input [%d]\n", i);
				printf("        scancode = %04x\n", input->m_scanCode);
				printf("        flags    = %04x\n", input->m_flags);
			}

			break;

		case dm_NotifyCode_MouseEvent:
			printf(
				"    input count:     %d\n",
				params.m_mouseEventParams->m_inputCount
			);

			for (size_t i = 0; i < params.m_mouseEventParams->m_inputCount; i++) {
				const dm_MouseInput* input = (const dm_MouseInput*) (params.m_mouseEventParams + 1) + i;
				printf("    input [%d]\n", i);
				printf("        flags    = %04x\n", input->m_flags);
				printf("        x        = %d\n", input->m_absoluteX);
				printf("        y        = %d\n", input->m_absoluteY);
			}

			break;

		default:
			printf(
				"    notify code      %d\n",
				notifyHdr->m_code
			);
		}

		if (notifyHdr->m_flags & dm_NotifyFlag_DataDropped)
			printf("*** DATA DROPPED\n");
	}
}

int
monitorDevice_mi(
	const sl::StringRef& deviceName,
	const sl::StringRef& fileNameWildcard
) {
	bool result;

	// switch stdout to binary mode without buffering

	_setmode(_fileno(stdout), _O_BINARY);
	setvbuf(stdout, NULL, _IONBF, 0);

	dm::Monitor monitor;
	result = monitor.open();
	if (!result) {
		err::pushError("Can't open " DM_MASTER_DEVICE_NAMEA);
		printLastError_mi();
		return Error_IoFailure;
	}

	uint_t version = monitor.getDriverVersion();
	if (version < 0x030300) {
		err::setFormatStringError(
			"Incompatible driver version (reported: %d.%d.%d; required: %d.%d.xx)\n",
			LOBYTE(HIWORD(version)),
			HIBYTE(LOWORD(version)),
			LOBYTE(LOWORD(version)),
			3,
			3
		);

		printLastError_mi();
		return Error_InvalidVersion;
	}

	result = monitor.connect(deviceName);
	if (!result) {
		err::pushFormatStringError("Can't connect to %s", deviceName.sz());
		printLastError_mi();
		return Error_IoFailure;
	}

	dm::DeviceInfo deviceInfo;

	result =
		monitor.getTargetDeviceInfo(&deviceInfo) &&
		monitor.setReadMode(dm_ReadMode_Message) &&
		monitor.setFileNameFilter(fileNameWildcard) &&
		monitor.enable();

	if (!result) {
		err::pushFormatStringError("Can't configure monitor on %s", deviceName.sz());
		printLastError_mi();
		return Error_IoFailure;
	}

	sl::String name = deviceInfo.m_deviceName;

	if (!fileNameWildcard.isEmpty()) {
		name += '\\';
		name += fileNameWildcard;
	}

	dm_MiStartParams params;
	params.m_version = version;
	params.m_flags =
		dm_MiStartFlag_Windows |

#if (_AXL_CPU_AMD64)
		dm_MiStartFlag_Amd64 |
#elif (_AXL_CPU_X86)
		dm_MiStartFlag_X86 |
#endif
		0;

	print_mi(dm_MiMsgCode_Start, &params, sizeof(params), name.cp(), name.getLength());

	// monitor EOF on STDIN (important for SSH connections)

	StdinThread stdinThread;
	result = stdinThread.start();

	sl::Array<char> buffer;
	buffer.setCount(1024); // reserve 1K

	for (;;) {
		size_t actualSize = monitor.read(buffer, buffer.getCount());
		if (actualSize == -1) {
			err::pushError("Read error");
			printLastError_mi();
			return Error_IoFailure;
		}

		dm_NotifyHdr* notifyHdr = (dm_NotifyHdr*)buffer.p();
		if (notifyHdr->m_flags & dm_NotifyFlag_InsufficientBuffer) {
			size_t bufferSize = sizeof(dm_NotifyHdr) + notifyHdr->m_paramSize;
			result = buffer.setCount(bufferSize);
			if (!result) {
				err::pushError("Read error");
				printLastError_mi();
				return false;
			}

			actualSize = monitor.read(buffer, bufferSize);
			if (actualSize == -1) {
				err::pushError("Read error");
				printLastError_mi();
				return Error_IoFailure;
			}
		}

		print_mi(dm_MiMsgCode_Notification, notifyHdr, sizeof(dm_NotifyHdr) + notifyHdr->m_paramSize);
	}
}

#elif (_AXL_OS_LINUX)

int
monitorDevice(
	const sl::StringRef& deviceName,
	const sl::StringRef& fileNameWildcard,
	const dm_IoctlDesc* ioctlDescArray,
	size_t ioctlDescCount
) {
	static const char* notifyCodeStringTable[dm_NotifyCode__Count] = {
		"<unknown>",      // dm_NotifyCode_Undefined,
		"open",           // dm_NotifyCode_Open,
		"close",          // dm_NotifyCode_Close,
		"read",           // dm_NotifyCode_Read,
		"write",          // dm_NotifyCode_Write,
		"unlocked_ioctl", // dm_NotifyCode_UnlockedIoctl,
		"compat_ioctl",   // dm_NotifyCode_CompatIoctl,
		"<data-dropped>", // dm_NotifyCode_DataDropped,
		"read_iter",      // dm_NotifyCode_ReadIter,
		"write_iter",     // dm_NotifyCode_WriteIter,
	};

	bool result;

	setvbuf(stdout, NULL, _IONBF, 0); // turn off buffering of stdout

	dm::Monitor monitor;
	result = monitor.open();
	if (!result) {
		printf("Can't open %s: %s\n", DM_DEVICE_NAME, err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	result = monitor.connect(deviceName);
	if (!result) {
		printf("Can't connect to %s: %s\n", deviceName.sz(), err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	dm::HookInfo hookInfo;

	result =
		monitor.getTargetHookInfo(&hookInfo) &&
		monitor.setReadMode(dm_ReadMode_Message) &&
		monitor.setFileNameFilter(fileNameWildcard) &&
		monitor.setIoctlDescTable(ioctlDescArray, ioctlDescCount) &&
		monitor.enable();

	if (!result) {
		printf("Can't configure %s: %s\n", deviceName.sz(), err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	printf(
		"Monitoring %s (module: %s)",
		hookInfo.m_fileName.sz(),
		!hookInfo.m_moduleName.isEmpty() ? hookInfo.m_moduleName.sz() : "<unknown>"
	);

	printf(", hit Ctrl+C to interrupt...\n");

	sl::Array<char> buffer;
	buffer.setCount(1024); // reserve 1K

	for (;;) {
		size_t actualSize = monitor.read(buffer, buffer.getCount());
		if (actualSize == -1) {
			printf("Read error: %s\n", err::getLastErrorDescription().sz());
			return Error_IoFailure;
		}

		dm_NotifyHdr* notifyHdr = (dm_NotifyHdr*)buffer.p();
		if (notifyHdr->m_flags & dm_NotifyFlag_InsufficientBuffer) {
			size_t bufferSize = sizeof(dm_NotifyHdr) + notifyHdr->m_paramSize;
			result = buffer.setCount(bufferSize);
			if (!result) {
				printf("Read error: %s\n", err::getLastErrorDescription().sz());
				return false;
			}

			actualSize = monitor.read(buffer, bufferSize);
			if (actualSize == -1) {
				printf("Read error: %s\n", err::getLastErrorDescription().sz());
				return Error_IoFailure;
			}
		}

		dm_NotifyParamsPtr params = { notifyHdr + 1 };
		size_t i = notifyHdr->m_code < countof(notifyCodeStringTable) ? notifyHdr->m_code : 0;
		sl::String timestampString = sys::Time(notifyHdr->m_timestamp).format("%h:%m:%s.%l");

		printf(
			"\n"
			"%s %s\n"
			"    pid:             %d\n"
			"    tid:             %d\n"
			"    result:          %d\n",
			timestampString.sz(),
			notifyCodeStringTable[i],
			notifyHdr->m_pid,
			notifyHdr->m_tid,
			notifyHdr->m_result
		);

		switch (notifyHdr->m_code) {
		case dm_NotifyCode_Open:
			printf(
				"    file id:         %016llx\n"
				"    flags:           %08x\n"
				"    mode:            %08x\n"
				"    file name:       %s\n",
				params.m_openParams->m_fileId,
				params.m_openParams->m_flags,
				params.m_openParams->m_mode,
				params.m_openParams + 1
			);

			break;

		case dm_NotifyCode_Close:
			printf(
				"    file id:         %016llx\n",
				params.m_closeParams->m_fileId
			);
			break;

		case dm_NotifyCode_Read:
		case dm_NotifyCode_Write:
		case dm_NotifyCode_ReadIter:
		case dm_NotifyCode_WriteIter:
			printf(
				"    file id:         %016llx\n"
				"    offset:          %lld\n"
				"    buffer size:     %d\n"
				"    data size:       %d\n",
				params.m_readWriteParams->m_fileId,
				params.m_readWriteParams->m_offset,
				params.m_readWriteParams->m_bufferSize,
				params.m_readWriteParams->m_dataSize
			);

			if (params.m_readWriteParams->m_dataSize) {
				printf("\n");
				printHexView(
					params.m_readWriteParams + 1,
					params.m_readWriteParams->m_dataSize
				);
			}

			break;

		case dm_NotifyCode_UnlockedIoctl:
		case dm_NotifyCode_CompatIoctl:
			printf(
				"    file id:         %016llx\n"
				"    ioctl code:      %08x\n"
				"    ioctl arg:       %016llx\n",
				params.m_ioctlParams->m_fileId,
				params.m_ioctlParams->m_code,
				params.m_ioctlParams->m_arg
			);

			if (params.m_ioctlParams->m_argSize) {
				printf(
					"    arg data size:   %d\n\n",
					params.m_ioctlParams->m_argSize
				);

				printHexView(
					params.m_ioctlParams + 1,
					params.m_ioctlParams->m_argSize
				);
			}

			break;

		default:
			printf(
				"    notify code      %d\n",
				notifyHdr->m_code
			);
		}

		if (notifyHdr->m_flags & dm_NotifyFlag_DataDropped)
			printf("*** DATA DROPPED\n");
	}
}

int
monitorDevice_mi(
	const sl::StringRef& deviceName,
	const sl::StringRef& fileNameWildcard,
	const dm_IoctlDesc* ioctlDescArray,
	size_t ioctlDescCount
) {
	bool result;

	setvbuf(stdout, NULL, _IONBF, 0); // turn off buffering of stdout

	dm::Monitor monitor;
	result = monitor.open();
	if (!result) {
		err::pushError("Can't open " DM_DEVICE_NAME);
		printLastError_mi();
		return Error_IoFailure;
	}

	result = monitor.connect(deviceName);
	if (!result) {
		err::pushFormatStringError("Can't connect to %s", deviceName.sz());
		printLastError_mi();
		return Error_IoFailure;
	}

	dm::HookInfo hookInfo;

	result =
		monitor.getTargetHookInfo(&hookInfo) &&
		monitor.setReadMode(dm_ReadMode_Message) &&
		monitor.setFileNameFilter(fileNameWildcard) &&
		monitor.setIoctlDescTable(ioctlDescArray, ioctlDescCount) &&
		monitor.enable();

	if (!result) {
		err::pushFormatStringError("Can't configure %s", deviceName.sz());
		printLastError_mi();
		return Error_IoFailure;
	}

	sl::String name = hookInfo.m_fileName;

	if (!hookInfo.m_moduleName.isEmpty()) {
		name += (utf32_t) '\0';
		name += hookInfo.m_moduleName;
	}

	dm_MiStartParams params;
	params.m_version = monitor.getDriverVersion();
	params.m_flags =
		dm_MiStartFlag_Posix |

#if (_AXL_OS_LINUX)
		dm_MiStartFlag_Linux |
#elif (_AXL_OS_BSD)
		dm_MiStartFlag_Bsd |
#elif (_AXL_OS_DARWIN)
		dm_MiStartFlag_Darwin |
#endif

#if (_AXL_CPU_AMD64)
		dm_MiStartFlag_Amd64 |
#elif (_AXL_CPU_X86)
		dm_MiStartFlag_X86 |
#elif (_AXL_CPU_ARM32)
		dm_MiStartFlag_Arm32 |
#elif (_AXL_CPU_ARM64)
		dm_MiStartFlag_Arm64 |
#endif
		0;

	print_mi(dm_MiMsgCode_Start, &params, sizeof(params), name.cp(), name.getLength());

	// monitor EOF on STDIN (important for SSH connections)

	StdinThread stdinThread;
	result = stdinThread.start();

	sl::Array<char> buffer;
	buffer.setCount(1024); // reserve 1K

	for (;;) {
		size_t actualSize = monitor.read(buffer, buffer.getCount());
		if (actualSize == -1) {
			err::pushError("Read error");
			printLastError_mi();
			return Error_IoFailure;
		}

		dm_NotifyHdr* notifyHdr = (dm_NotifyHdr*)buffer.p();
		if (notifyHdr->m_flags & dm_NotifyFlag_InsufficientBuffer) {
			size_t bufferSize = sizeof(dm_NotifyHdr) + notifyHdr->m_paramSize;
			result = buffer.setCount(bufferSize);
			if (!result) {
				err::pushError("Read error");
				printLastError_mi();
				return false;
			}

			actualSize = monitor.read(buffer, bufferSize);
			if (actualSize == -1) {
				err::pushError("Read error");
				printLastError_mi();
				return Error_IoFailure;
			}
		}

		print_mi(dm_MiMsgCode_Notification, notifyHdr, sizeof(dm_NotifyHdr) + notifyHdr->m_paramSize);
	}
}

#endif

//..............................................................................
