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

add_subdirectory(dm_app)

if(WIN32)
	add_subdirectory(dm_win_lib)
	add_subdirectory(dm_win_lib_libcmt)

	if(WDK_FOUND)
		add_subdirectory(dm_win_sys_core)
		add_subdirectory(dm_win_sys_pnp)
		add_subdirectory(dm_win_sys_cmn)
	endif()
endif()

if(LINUX)
	add_subdirectory(dm_lnx_lib)
	add_subdirectory(dm_lnx_lkm)
endif()

#...............................................................................
