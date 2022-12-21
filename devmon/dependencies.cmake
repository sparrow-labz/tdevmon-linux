#...............................................................................
#
#  This file is part of Tibbo Device Monitor.
#  Copyright (c) 2014-2017, Tibbo Technology Inc
#  Author: Vladimir Gladkov
#
#  For legal details see accompanying license.txt file,
#  the public copy of which is also available at:
#  http://tibbo.com/downloads/archive/tdevmon/license.txt
#
#...............................................................................

set(
	AXL_PATH_LIST

	AXL_CMAKE_DIR
	7Z_EXE
	SIGNTOOL_EXE
	WIX_BIN_DIR
	WDK_BIN_DIR
	WDK_CRT_INC_DIR
	WDK_SDK_INC_DIR
	WDK_DDK_INC_DIR
	WDK_CRT_LIB_DIR
	WDK_DDK_LIB_DIR
	LINUX_KERNEL_BUILD_DIR
)

set(
	AXL_IMPORT_LIST

	REQUIRED
		axl
	OPTIONAL
		wdk
		wix
		7z
	)

#...............................................................................
