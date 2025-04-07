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

configure_file(
	devmon_config_build.cmake.in
	devmon_config.cmake
	@ONLY
)

configure_file(
	devmon_config_install.cmake.in
	devmon_config_install.cmake
	@ONLY
)

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

install(
	FILES import_devmon.cmake
	DESTINATION DEVMON_INSTALL_CMAKE_SUBDIR=/home/sparrow/tdevmon-linux/build/devmon/bin/Release
	COMPONENT cmake
)

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/devmon_config_install.cmake
	RENAME devmon_config.cmake
	DESTINATION DEVMON_INSTALL_CMAKE_SUBDIR=/home/sparrow/tdevmon-linux/build/devmon/bin/Release
	COMPONENT cmake
)

install(
	FILES ${DEVMON_ROOT_DIR}/version.cmake
	RENAME devmon_version.cmake
	DESTINATION DEVMON_INSTALL_CMAKE_SUBDIR=/home/sparrow/tdevmon-linux/build/devmon/bin/Release
	COMPONENT cmake
)

#...............................................................................
