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

#
# src folder
#

set(
	SRC_H_LIST
	${DEVMON_INC_DIR}/dm_MachineIface.h
	AccessMgr.h
	CmdLine.h
	error.h
	stats.h
	service.h
	pnp.h
	security.h
	monitor.h
)

set(
	SRC_CPP_LIST
	AccessMgr.cpp
	CmdLine.cpp
	main.cpp
	stats.cpp
	service.cpp
	pnp.cpp
	security.cpp
	monitor.cpp
)

source_group(
	src
	FILES
	${SRC_H_LIST}
	${SRC_CPP_LIST}
	${SRC_RL_LIST}
)

if(NOT WIN32)
	axl_exclude_from_build(
		AccessMgr.cpp
		service.cpp
		pnp.cpp
		security.cpp
		test.cpp
	)
endif()

if(NOT LINUX)
	axl_exclude_from_build(
		unhook.cpp
	)
endif()

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# res folder
#

set(
	RES_RC_LIST
	res.rc
)

source_group(
	res
	FILES
	${RES_RC_LIST}
)

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# gen folder
#

set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen)
file(MAKE_DIRECTORY ${GEN_DIR})

axl_push_and_set(CMAKE_CURRENT_BINARY_DIR ${GEN_DIR})

configure_file(
	version.h.in
	${GEN_DIR}/version.h
)

set(
	GEN_H_LIST
	${GEN_DIR}/version.h
)

axl_pop(CMAKE_CURRENT_BINARY_DIR)

source_group(
	gen
	FILES
	${GEN_H_LIST}
)

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# pch folder
#

set(PCH_H   pch.h)
set(PCH_CPP pch.cpp)

source_group(
	pch
	FILES
	${PCH_H}
	${PCH_CPP}
)

#...............................................................................
#
# dm_app monitor app
#

include_directories(
	${AXL_INC_DIR}
	${DEVMON_INC_DIR}
	${GEN_DIR}
)

link_directories(${AXL_LIB_DIR})

add_executable(
	dm_app
	${PCH_H}
	${PCH_CPP}
	${SRC_H_LIST}
	${SRC_CPP_LIST}
	${RES_RC_LIST}
	${GEN_H_LIST}
)

axl_set_pch(
	dm_app
	${PCH_H}
	${PCH_CPP}
)

set_target_properties(
	dm_app
	PROPERTIES
	OUTPUT_NAME tdevmon
)

if(WIN32)
	target_link_libraries(
		dm_app
		dm_win_lib
		setupapi
		netapi32
		axl_io
		axl_core
	)
elseif(LINUX)
	target_link_libraries(
		dm_app
		dm_lnx_lib
		axl_io
		axl_core
		pthread
		rt
	)
endif()

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

install(
	TARGETS dm_app
	DESTINATION ${DEVMON_INSTALL_BIN_SUBDIR}
	COMPONENT bin
)

#...............................................................................
