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

axl_find_file(
	_CONFIG_CMAKE
	devmon_config.cmake
	${DEVMON_CMAKE_DIR}
)

if(_CONFIG_CMAKE)
	include(${_CONFIG_CMAKE})

	message(STATUS "DevMon ${DEVMON_VERSION_FULL} paths:")
	axl_message("    DevMon cmake files:" "${DEVMON_CMAKE_DIR}")
	axl_message("    DevMon includes:"    "${DEVMON_INC_DIR}")
	axl_message("    DevMon libraries:"   "${DEVMON_LIB_DIR}")

	set(DEVMON_FOUND TRUE)
else()
	set(DEVMON_FOUND FALSE)
endif()

#...............................................................................
