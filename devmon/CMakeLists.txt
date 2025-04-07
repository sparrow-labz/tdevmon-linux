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

project(devmon)

cmake_minimum_required(VERSION 3.4)

include(cmake/axl_cmake.cmake NO_POLICY_SCOPE)

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

set(DEVMON_ROOT_DIR     ${CMAKE_CURRENT_LIST_DIR})
set(DEVMON_BUILD_DIR    ${CMAKE_CURRENT_BINARY_DIR})
set(DEVMON_BIN_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(DEVMON_LIB_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(DEVMON_DOC_DIR      ${CMAKE_CURRENT_BINARY_DIR}/doc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DEVMON_BIN_BASE_DIR}/${CONFIGURATION_SCG})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DEVMON_LIB_BASE_DIR}/${CONFIGURATION_SCG})

set(DEVMON_INSTALL_BIN_SUBDIR bin)
set(DEVMON_INSTALL_LIB_SUBDIR lib)
set(DEVMON_INSTALL_DRIVER_SUBDIR drivers)

if(WIN32)
	set(DEVMON_INSTALL_INC_SUBDIR       include)
	set(DEVMON_INSTALL_LIB_SUBDIR       lib)
	set(DEVMON_INSTALL_CMAKE_SUBDIR     cmake)
	set(DEVMON_INSTALL_DRIVER_SUBDIR    drivers)
	set(DEVMON_INSTALL_LICENSE_SUBDIR   license)
	set(DEVMON_INSTALL_CHANGELOG_SUBDIR .)
elseif(LINUX)
	set(DEVMON_INSTALL_INC_SUBDIR       include/tdevmon)
	set(DEVMON_INSTALL_CMAKE_SUBDIR     lib/cmake/tdevmon)
	set(DEVMON_INSTALL_LICENSE_SUBDIR   share/tdevmon/license)
	set(DEVMON_INSTALL_CHANGELOG_SUBDIR share/tdevmon)
	set(DEVMON_INSTALL_LKM_SRC_SUBDIR   share/tdevmon/src/lkm)
	set(DEVMON_INSTALL_LKM_SRC_TO_ROOT  ../../../..)
endif()

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

if(WIN32)
	# what a mess with CPU names in WDK...

	if("${TARGET_CPU}" STREQUAL "amd64")
		set(WDK_CPU     "x64")
		set(WDK_CPU_DIR "amd64")
	elseif("${TARGET_CPU}" STREQUAL "x86")
		set(WDK_CPU     "x86")
		set(WDK_CPU_DIR "i386")
	endif()
endif()

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

add_subdirectory(cmake)

include(${CMAKE_CURRENT_BINARY_DIR}/cmake/devmon_config.cmake)

add_subdirectory(src)
add_subdirectory(package)

if(WIN32)
	add_subdirectory(whdc)
endif()

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

if(WIN32)
	set(
		_CORE_H_LIST
		include/dm_win_Protocol.h
	)

	set(
		_EXTRA_H_LIST
		include/dm_win_DeviceMgr.h
		include/dm_win_Monitor.h
		include/dm_win_Pch.h
		include/dm_win_PnpBeacon.h
		include/dm_win_Service.h
	)
elseif(LINUX)
	set(
		_CORE_H_LIST
		include/dm_lnx_Protocol.h
	)

	set(
		_EXTRA_H_LIST
		include/dm_lnx_Monitor.h
		include/dm_lnx_Pch.h
	)
endif()

list(
	APPEND
	_CORE_H_LIST
	include/dm_MachineIface.h
)

install(
	FILES ${_CORE_H_LIST}
	DESTINATION DEVMON_INSTALL_CMAKE_SUBDIR=/home/sparrow/tdevmon-linux/build/devmon/bin/Release
	COMPONENT include
)

install(
	FILES ${_EXTRA_H_LIST}
	DESTINATION ${DEVMON_INSTALL_INC_SUBDIR}
	COMPONENT include_extra
)

install(
	DIRECTORY license/
	DESTINATION DEVMON_INSTALL_CMAKE_SUBDIR=/home/sparrow/tdevmon-linux/build/devmon/bin/Release
	COMPONENT license
)

install(
	FILES changelog.txt
	DESTINATION DEVMON_INSTALL_CMAKE_SUBDIR=/home/sparrow/tdevmon-linux/build/devmon/bin/Release
	COMPONENT changelog
)

#...............................................................................

set(CPACK_PACKAGE_VERSION_MAJOR ${DEVMON_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${DEVMON_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${DEVMON_VERSION_REVISION})

if(WIN32)
	set(CPACK_SOURCE_GENERATOR 7Z)
else()
	set(CPACK_SOURCE_GENERATOR TXZ)
endif()

set(CPACK_SOURCE_IGNORE_FILES "/[.];[.]user$;/build/;/ci/;/paths.cmake$")

set(CPACK_SOURCE_PACKAGE_FILE_NAME tdevmon-${DEVMON_VERSION_FILE_SUFFIX}-src)

string(TOLOWER ${CMAKE_SYSTEM_NAME} OS_NAME)
set(CPACK_PACKAGE_FILE_NAME tdevmon-${DEVMON_VERSION_FILE_SUFFIX}-${OS_NAME}-${TARGET_CPU})

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY ON)
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)

set(
	CPACK_COMPONENTS_ALL
	bin
	kernel
	include
	license
	changelog
	# currently don't install pre-built libraries and cmake files
)

include(CPack)

#...............................................................................
