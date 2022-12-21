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
# lib folder
#

set(
	LIB_H_LIST
	${DEVMON_INC_DIR}/dm_lnx_Protocol.h
	${DEVMON_INC_DIR}/dm_lnx_Monitor.h
	${DEVMON_INC_DIR}/dm_lnx_Pch.h
)

set(
	LIB_CPP_LIST
	dm_lnx_Monitor.cpp
)

source_group(
	lib
	FILES
	${LIB_H_LIST}
	${LIB_CPP_LIST}
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
# dm_lnx_lib library
#

include_directories(
	${AXL_INC_DIR}
	${DEVMON_INC_DIR}
)

add_library(
	dm_lnx_lib
	${PCH_H}
	${PCH_CPP}
	${LIB_H_LIST}
	${LIB_CPP_LIST}
)

axl_set_pch(
	dm_lnx_lib
	${PCH_H}
	${PCH_CPP}
)

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

install(
	TARGETS dm_lnx_lib
	DESTINATION ${DEVMON_INSTALL_LIB_SUBDIR}
	COMPONENT lib
)

#...............................................................................
