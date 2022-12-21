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

if(WIN32)
	add_subdirectory(msi)
	add_subdirectory(actions)
endif()

set(_DEBUG_SUFFIX $<$<CONFIG:Debug>:d>)

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

# VC runtime

if(${CMAKE_GENERATOR} MATCHES "Visual Studio 10")
	install(
		FILES
			redist/${TARGET_CPU}/msvcr100${_DEBUG_SUFFIX}.dll
			redist/${TARGET_CPU}/msvcp100${_DEBUG_SUFFIX}.dll
		DESTINATION ${DEVMON_INSTALL_BIN_SUBDIR}
		COMPONENT bin
	)
endif()

#...............................................................................
