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

#ifndef __cplusplus
typedef struct dm_DeviceInfo                  dm_DeviceInfo;
typedef struct dm_DeviceInfoList              dm_DeviceInfoList;
typedef enum dm_ReadMode                      dm_ReadMode;
typedef struct dm_ConnectParams_v0300xx       dm_ConnectParams_v0300xx;
typedef struct dm_ConnectParams_v0302xx       dm_ConnectParams_v0302xx;
typedef enum dm_NotifyCode                    dm_NotifyCode;
typedef struct dm_NotifyHdr                   dm_NotifyHdr;
typedef struct dm_NotifyHdr_v0300xx           dm_NotifyHdr_v0300xx;
typedef struct dm_CreateNotifyParams          dm_CreateNotifyParams;
typedef struct dm_CreateNamedPipeNotifyParams dm_CreateNamedPipeNotifyParams;
typedef struct dm_CreateMailslotNotifyParams  dm_CreateMailslotNotifyParams;
typedef struct dm_CloseNotifyParams           dm_CloseNotifyParams;
typedef struct dm_ReadWriteNotifyParams       dm_ReadWriteNotifyParams;
typedef struct dm_IoctlNotifyParams           dm_IoctlNotifyParams;
typedef enum dm_KeyboardEventFlag             dm_KeyboardEventFlag;
typedef enum dm_MouseEventFlag                dm_MouseEventFlag;
typedef struct dm_KeyboardEventParams         dm_KeyboardEventParams;
typedef struct dm_KeyboardInput               dm_KeyboardInput;
typedef struct dm_MouseEventParams            dm_MouseEventParams;
typedef struct dm_MouseInput                  dm_MouseInput;

typedef union dm_NotifyParams                 dm_NotifyParams;
typedef union dm_NotifyParamsPtr              dm_NotifyParamsPtr;
#endif

//..............................................................................

enum {
	dm_ConnectionCountLimit      = 16,              // no more than 16 connections to a device
	dm_DefPendingNotifySizeLimit = 1 * 1024 * 1024, // drop notifications if application is not fast enough to pick'em up
	dm_NotifyHdrSignature        = 'nomt',
	dm_DeviceType                = FILE_DEVICE_UNKNOWN,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

#define DM_MASTER_DEVICE_NAMEA        "tdevmon-master"
#define DM_BEACON_DEVICE_NAMEA        "tdevmon-beacon"
#define DM_CORE_DRIVER_FILE_NAMEA     "tdevmonc.sys"
#define DM_CORE_SERVICE_NAMEA         "tdevmonc"
#define DM_CORE_SERVICE_DISPLAY_NAMEA "Tibbo Device Monitor kernel-mode core service"
#define DM_CORE_SERVICE_DESCRIPTIONA  "Provides core monitoring capabilities for Tibbo IO Ninja"
#define DM_CORE_SERVICE_REG_PARAM_SDA "sd"
#define DM_PNP_DRIVER_FILE_NAMEA      "tdevmonp.sys"
#define DM_PNP_SERVICE_NAMEA          "tdevmonp"
#define DM_PNP_SERVICE_DISPLAY_NAMEA  "Tibbo Device Monitor kernel-mode PNP filter service"
#define DM_PNP_SERVICE_DESCRIPTIONA   "Provides early-attach to PNP devices for Tibbo IO Ninja"

#define DM_MASTER_DEVICE_NAMEW        _CRT_WIDE(DM_MASTER_DEVICE_NAMEA)
#define DM_BEACON_DEVICE_NAMEW        _CRT_WIDE(DM_BEACON_DEVICE_NAMEA)
#define DM_CORE_DRIVER_FILE_NAMEW     _CRT_WIDE(DM_CORE_DRIVER_FILE_NAMEA)
#define DM_CORE_SERVICE_NAMEW         _CRT_WIDE(DM_CORE_SERVICE_NAMEA)
#define DM_CORE_SERVICE_DISPLAY_NAMEW _CRT_WIDE(DM_CORE_SERVICE_DISPLAY_NAMEA)
#define DM_CORE_SERVICE_DESCRIPTIONW  _CRT_WIDE(DM_CORE_SERVICE_DESCRIPTIONA)
#define DM_CORE_SERVICE_REG_PARAM_SDW _CRT_WIDE(DM_CORE_SERVICE_REG_PARAM_SDA)
#define DM_PNP_DRIVER_FILE_NAMEW      _CRT_WIDE(DM_PNP_DRIVER_FILE_NAMEA)
#define DM_PNP_SERVICE_NAMEW          _CRT_WIDE(DM_PNP_SERVICE_NAMEA)
#define DM_PNP_SERVICE_DISPLAY_NAMEW  _CRT_WIDE(DM_PNP_SERVICE_DISPLAY_NAMEA)
#define DM_PNP_SERVICE_DESCRIPTIONW   _CRT_WIDE(DM_PNP_SERVICE_DESCRIPTIONA)

#define DM_MASTER_DEVICE_NAME         _T(DM_MASTER_DEVICE_NAMEA)
#define DM_BEACON_DEVICE_NAME         _T(DM_BEACON_DEVICE_NAMEA)
#define DM_CORE_DRIVER_FILE_NAME      _T(DM_CORE_DRIVER_FILE_NAMEA)
#define DM_CORE_SERVICE_NAME          _T(DM_CORE_SERVICE_NAMEA)
#define DM_CORE_SERVICE_DISPLAY_NAME  _T(DM_CORE_SERVICE_DISPLAY_NAMEA)
#define DM_CORE_SERVICE_DESCRIPTION   _T(DM_CORE_SERVICE_DESCRIPTIONA)
#define DM_CORE_SERVICE_REG_PARAM_SD  _T(DM_CORE_SERVICE_REG_PARAM_SDA)
#define DM_PNP_DRIVER_FILE_NAME       _T(DM_PNP_DRIVER_FILE_NAMEA)
#define DM_PNP_SERVICE_NAME           _T(DM_PNP_SERVICE_NAMEA)
#define DM_PNP_SERVICE_DISPLAY_NAME   _T(DM_PNP_SERVICE_DISPLAY_NAMEA)
#define DM_PNP_SERVICE_DESCRIPTION    _T(DM_PNP_SERVICE_DESCRIPTIONA)

//..............................................................................

#define DM_CTL_CODE_EX(code, method) \
	CTL_CODE(dm_DeviceType, 0x800 + code, method, FILE_ANY_ACCESS)

#define DM_CTL_CODE(code) \
	DM_CTL_CODE_EX(code, METHOD_BUFFERED)

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

#define DM_IOCTL_GET_VERSION            DM_CTL_CODE(1)
#define DM_IOCTL_GET_DESCRIPTION        DM_CTL_CODE(2)
#define DM_IOCTL_GET_BUILD_TIME         DM_CTL_CODE(3)
#define DM_IOCTL_GET_DEVICE_INFO_LIST   DM_CTL_CODE(4)
#define DM_IOCTL_GET_TARGET_DEVICE_INFO DM_CTL_CODE(5)
#define DM_IOCTL_IS_CONNECTED           DM_CTL_CODE(6)
#define DM_IOCTL_CONNECT_V0300XX        DM_CTL_CODE(7)
#define DM_IOCTL_DISCONNECT             DM_CTL_CODE(8)
#define DM_IOCTL_STOP                   DM_CTL_CODE(9)
#define DM_IOCTL_CONNECT_V0302XX        DM_CTL_CODE(10)
#define DM_IOCTL_CHECK_ACCESS           DM_CTL_CODE(11)
#define DM_IOCTL_CONNECT                DM_CTL_CODE(12)
#define DM_IOCTL_IS_ENABLED             DM_CTL_CODE(13)
#define DM_IOCTL_ENABLE                 DM_CTL_CODE(14)
#define DM_IOCTL_DISABLE                DM_CTL_CODE(15)
#define DM_IOCTL_GET_READ_MODE          DM_CTL_CODE(16)
#define DM_IOCTL_SET_READ_MODE          DM_CTL_CODE(17)
#define DM_IOCTL_GET_FILE_NAME_FILTER   DM_CTL_CODE(18)
#define DM_IOCTL_SET_FILE_NAME_FILTER   DM_CTL_CODE(19)
#define DM_IOCTL_GET_PENDING_NOTIFY_SIZE_LIMIT DM_CTL_CODE(20)
#define DM_IOCTL_SET_PENDING_NOTIFY_SIZE_LIMIT DM_CTL_CODE(21)

#if (DBG || defined _DEBUG || defined _AXL_DEBUG)
#	define DM_IOCTL_TEST                DM_CTL_CODE(50)
#endif

//..............................................................................

enum dm_DeviceFlag {
	dm_DeviceFlag_RemotePnpFilter = 0x01,
	dm_DeviceFlag_PnpDevice       = 0x02,
	dm_DeviceFlag_FilterConnected = 0x04,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct dm_DeviceInfo {
	ULONG m_flags; // dm_DeviceFlag combination
	ULONG m_type;  // FILE_DEVICE_xxx
	ULONG m_connectionCount;
	ULONG m_requestCount;
	ULONG m_nameLength;

	// followed by a UTF16 device name;
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct dm_DeviceInfoList {
	ULONG m_deviceCount;
	ULONG m_deviceInfoSize;

	// followed by dm_DeviceInfo x m_deviceCount
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

enum dm_ReadMode {
	dm_ReadMode_Undefined = 0,
	dm_ReadMode_Stream,
	dm_ReadMode_Message,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct dm_ConnectParams_v0300xx {
	ULONG m_pendingNotifySizeLimit; // 0 -- use dm_DefPendingNotifySizeLimit

	// followed by a UTF16 device name (full device name or a symbolic link)
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct dm_ConnectParams_v0302xx {
	ULONG m_pendingNotifySizeLimit; // 0 -- use dm_DefPendingNotifySizeLimit
	ULONG m_deviceNameLength;
	ULONG m_fileNameWildcardLength;

	// followed by a UTF16 device name (full device name or a symbolic link)
	// followed by a UTF16 file name wildcard filter (no wildcard if m_wildcardLength == 0)
};

//..............................................................................

enum dm_NotifyCode {
	dm_NotifyCode_Undefined = 0,
	dm_NotifyCode_PnpStartDevice,
	dm_NotifyCode_PnpStopDevice,
	dm_NotifyCode_PnpRemoveDevice,
	dm_NotifyCode_FastIoDetachDevice,
	dm_NotifyCode_Create,
	dm_NotifyCode_CreateNamedPipe,
	dm_NotifyCode_Close,
	dm_NotifyCode_Read,
	dm_NotifyCode_Write,
	dm_NotifyCode_Ioctl,
	dm_NotifyCode_InternalIoctl,
	dm_NotifyCode_Fsctl,
	dm_NotifyCode_CreateMailslot,
	dm_NotifyCode_KeyboardEvent,
	dm_NotifyCode_MouseEvent,
	dm_NotifyCode_FastIoRead,
	dm_NotifyCode_FastIoWrite,
	dm_NotifyCode_FastIoIoctl,
	dm_NotifyCode_DataDropped, // a dedicated notification code
	dm_NotifyCode__Count,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

enum dm_NotifyFlag {
	dm_NotifyFlag_InsufficientBuffer = 0x01, // buffer is not big enough, resize and try again
	dm_NotifyFlag_DataDropped        = 0x02, // one or more notifications after this one were dropped
	dm_NotifyFlag_Timestamp          = 0x04, // this notification is timestamped
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct dm_NotifyHdr_v0300xx {
	UINT m_signature;
	USHORT m_code;
	USHORT m_flags;
	LONG m_ntStatus;
	ULONG m_paramSize;

	// followed by params
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct dm_NotifyHdr {
	UINT m_signature;
	USHORT m_code;
	USHORT m_flags;
	LONG m_ntStatus;
	ULONG m_paramSize;
	ULONG m_processId;
	ULONG m_threadId;
	ULONGLONG m_timestamp;

	// followed by params
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

// unions and paddings are to ensure the same layout for x86/amd64 and regardless of pragma pack

struct dm_CreateNotifyParams {
	union {
		UINT_PTR m_fileId;
		ULONGLONG m_fileId64;
	};

	UINT m_options;
	UINT m_desiredAccess;
	UINT m_shareAccess;
	UINT m_fileAttributes;
	ULONG m_fileNameLength;
	ULONG _m_padding;

	// followed by UTF16 file name
};

#ifdef __cplusplus
struct dm_CreateNamedPipeNotifyParams: dm_CreateNotifyParams {
#else
struct dm_CreateNamedPipeNotifyParams { dm_CreateNotifyParams;
#endif

	UINT m_pipeType;
	UINT m_readMode;
	UINT m_completionMode;
	ULONG m_maxInstanceCount;
	ULONG m_inBoundQuota;
	ULONG m_outBoundQuota;
	ULONGLONG m_defaultTimeout;

	// followed by UTF16 pipe name
};

#ifdef __cplusplus
struct dm_CreateMailslotNotifyParams: dm_CreateNotifyParams {
#else
struct dm_CreateMailslotNotifyParams { dm_CreateNotifyParams;
#endif

	ULONG m_mailslotQuota;
	ULONG m_maxMessageSize;
	ULONGLONG m_readTimeout;

	// followed by UTF16 mailslot name
};

struct dm_CloseNotifyParams {
	union {
		UINT_PTR m_fileId;
		ULONGLONG m_fileId64;
	};
};

struct dm_ReadWriteNotifyParams {
	union {
		UINT_PTR m_fileId;
		ULONGLONG m_fileId64;
	};

	ULONGLONG m_offset;
	UINT m_bufferSize;
	UINT m_dataSize;

	// followed by read/write data
};

struct dm_IoctlNotifyParams {
	union {
		UINT_PTR m_fileId;
		ULONGLONG m_fileId64;
	};

	UINT m_code;
	ULONG m_inDataSize;
	ULONG m_outBufferSize;
	ULONG m_outDataSize;

	// followed by in-data
	// followed by out-data
};

enum dm_MouseEventFlag {
	dm_MouseEventFlag_RelativeCoordinates = 0x0001,
	dm_MouseEventFlag_AbsoluteCoordinates = 0x0002,
	dm_MouseEventFlag_VirtualDesktop      = 0x0004,
	dm_MouseEventFlag_MoveNoCoalesce      = 0x0008,
	dm_MouseEventFlag_LeftButtonDown      = 0x0010,
	dm_MouseEventFlag_LeftButtonUp        = 0x0020,
	dm_MouseEventFlag_RightButtonDown     = 0x0040,
	dm_MouseEventFlag_RightButtonUp       = 0x0080,
	dm_MouseEventFlag_MiddleButtonDown    = 0x0100,
	dm_MouseEventFlag_MiddleButtonUp      = 0x0200,
	dm_MouseEventFlag_Button4Down         = 0x0400,
	dm_MouseEventFlag_Button4Up           = 0x0800,
	dm_MouseEventFlag_Button5Down         = 0x1000,
	dm_MouseEventFlag_Button5Up           = 0x2000,
	dm_MouseEventFlag_Wheel               = 0x4000,
};

enum dm_KeyboardEventFlag {
	dm_KeyboardEventFlag_Pressed           = 0x01,
	dm_KeyboardEventFlag_Released          = 0x02,
	dm_KeyboardEventFlag_ExtendedScanCode  = 0x04,
	dm_KeyboardEventFlag_ExtendedScanCode2 = 0x08,
};

struct dm_KeyboardEventParams {
	ULONG m_inputCount;

	// followed by dm_KeyboardInput m_input [m_inputCount]
};

struct dm_KeyboardInput { // binary compatible with WDK KEYBOARD_INPUT_DATA
	USHORT m_unitId;
	USHORT m_scanCode;
	USHORT m_flags;
	USHORT m_reserved;
	ULONG  m_extraInformation;
};

struct dm_MouseEventParams {
	ULONG m_inputCount;

	// followed by dm_MouseInput m_input [m_inputCount]
};

struct dm_MouseInput { // binary compatible with WDK MOUSE_INPUT_DATA
	USHORT m_unitId;
	USHORT m_flags;

	union {
		ULONG m_buttons;

		struct {
			USHORT m_buttonFlags;
			USHORT m_buttonData;
		};
	};

	ULONG  m_rawButtons;

	union {
		struct {
			LONG m_absoluteX;
			LONG m_absoluteY;
		};

		struct {
			LONG m_deltaX;
			LONG m_deltaY;
		};
	};

	ULONG  m_extraInformation;
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

union dm_NotifyParams {
	dm_CreateNotifyParams m_createParams;
	dm_CreateNamedPipeNotifyParams m_createNamedPipeParams;
	dm_CreateMailslotNotifyParams m_createMailslotParams;
	dm_CloseNotifyParams m_closeParams;
	dm_ReadWriteNotifyParams m_readWriteParams;
	dm_IoctlNotifyParams m_ioctlParams;
	dm_MouseEventParams m_mouseEventParams;
	dm_KeyboardEventParams m_keyboardEventParams;
};

union dm_NotifyParamsPtr {
	VOID* m_params;
	dm_CreateNotifyParams* m_createParams;
	dm_CreateNamedPipeNotifyParams* m_createNamedPipeParams;
	dm_CreateMailslotNotifyParams* m_createMailslotParams;
	dm_CloseNotifyParams* m_closeParams;
	dm_ReadWriteNotifyParams* m_readWriteParams;
	dm_IoctlNotifyParams* m_ioctlParams;
	dm_MouseEventParams* m_mouseEventParams;
	dm_KeyboardEventParams* m_keyboardEventParams;
};

#ifdef _NTDDK_

//..............................................................................

#ifndef __cplusplus
typedef struct dm_CoreDriverDispatch dm_CoreDriverDispatch;
typedef struct dm_PnpDriverDispatch  dm_PnpDriverDispatch;
#endif

//..............................................................................

// these are sent as internal IOCTLs

#define DM_IOCTL_CORE_TO_PNP_DRIVER_CONNECT DM_CTL_CODE_EX(100, METHOD_NEITHER)
#define DM_IOCTL_PNP_TO_CORE_DRIVER_CONNECT DM_CTL_CODE_EX(101, METHOD_NEITHER)

//..............................................................................

typedef
VOID
dm_DisconnectPnpDriverFunc();

typedef
NTSTATUS
dm_CreateFilterDeviceFunc(
	DEVICE_OBJECT** filterDeviceObject,
	DEVICE_OBJECT* targetDeviceObject,
	const WCHAR* targetDeviceName
);

typedef
VOID
dm_AddRefFilterDeviceFunc(DEVICE_OBJECT* deviceObject);

typedef
VOID
dm_ReleaseFilterDeviceFunc(DEVICE_OBJECT* deviceObject);

typedef
VOID
dm_OnKeyboardEventFunc(
	DEVICE_OBJECT* deviceObject,
	KEYBOARD_INPUT_DATA* dataStart,
	KEYBOARD_INPUT_DATA* dataEnd
);

typedef
VOID
dm_OnMouseEventFunc(
	DEVICE_OBJECT* deviceObject,
	MOUSE_INPUT_DATA* dataStart,
	MOUSE_INPUT_DATA* dataEnd
);

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct dm_CoreDriverDispatch {
	dm_DisconnectPnpDriverFunc* m_disconnectPnpDriver;
	dm_CreateFilterDeviceFunc* m_createFilterDevice;
	dm_AddRefFilterDeviceFunc* m_addRefFilterDevice;
	dm_ReleaseFilterDeviceFunc* m_releaseFilterDevice;
	dm_OnKeyboardEventFunc* m_onKeyboardEvent;
	dm_OnMouseEventFunc* m_onMouseEvent;
	VOID* m_nullTerminator;
};

//..............................................................................

typedef
VOID
dm_DisconnectCoreDriverFunc();

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct dm_PnpDriverDispatch {
	dm_DisconnectCoreDriverFunc* m_disconnectCoreDriver;
};

//..............................................................................

#endif
