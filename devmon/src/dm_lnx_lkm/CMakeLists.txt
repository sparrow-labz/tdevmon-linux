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
# lkm folder
#

set(
	LKM_PUBLIC_H_LIST
	${DEVMON_INC_DIR}/dm_lnx_Protocol.h
)

set(
	LKM_H_LIST
	pch.h
	Device.h
	Hook.h
	Connection.h
	HashTable.h
	ScatterGather.h
	FileNameFilter.h
	lkmUtils.h
	stringUtils.h
	typedefs.h
)

set(
	LKM_C_LIST
	module.c
	Device.c
	Hook.c
	Connection.c
	HashTable.c
	ScatterGather.c
	FileNameFilter.c
	lkmUtils.c
	stringUtils.c
)

set(
	LKM_MAKEFILE_LIST
	CMakeMakefile.in
)

source_group(
	lkm
	FILES
	${LKM_PUBLIC_H_LIST}
	${LKM_H_LIST}
	${LKM_CPP_LIST}
	${LKM_MAKEFILE_LIST}
)

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# pch folder
#

set(PCH_H pch.h)
set(PCH_C pch.c)

source_group(
	pch
	FILES
	${PCH_H}
	${PCH_C}
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
# kbuild directory with proper kernel makefiles
#

if(LINUX_KERNEL_BUILD_DIR)
	set(MAKE_BUILD_DIR ${LINUX_KERNEL_BUILD_DIR})
else()
	set(MAKE_BUILD_DIR "/lib/modules/$(shell uname -r)/build/")
endif()

set(LKM_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/kbuild)
set(LKM_OBJS)

foreach(_FILE ${LKM_C_LIST})
	get_filename_component(_FILE_NAME ${_FILE} NAME_WE)
	set(LKM_OBJS "${LKM_OBJS} ${_FILE_NAME}.o")
endforeach()

set(LKM_NAME tdevmon)

configure_file(
	CMakeMakefile.in
	${LKM_BUILD_DIR}/Makefile
)

configure_file(
	Makefile.in
	${LKM_BUILD_DIR}/Makefile_install
)

#...............................................................................
#
# dm_lnx_lkm Linux loadable kernel module
#

option(
	BUILD_DM_LNX_LKM
	"Generate a library target for tdevmon LKM (for IDEs)"
	OFF
)

if(BUILD_DM_LNX_LKM)

	# these are extracted from `make V=1` during THE REAL kernel module build
	# the goal is to comfortably edit kernel module sources from IDEs such as CLion

	if(LINUX_KERNEL_BUILD_DIR)
		set(_LINUX_KERNEL_BUILD_DIR ${LINUX_KERNEL_BUILD_DIR})
	else()
		set(_LINUX_KERNEL_BUILD_DIR /lib/modules/${CMAKE_SYSTEM_VERSION}/build)
	endif()

	include_directories(
		${_LINUX_KERNEL_BUILD_DIR}/arch/x86/include
		${_LINUX_KERNEL_BUILD_DIR}/arch/x86/include/generated
		${_LINUX_KERNEL_BUILD_DIR}/include
		${_LINUX_KERNEL_BUILD_DIR}/arch/x86/include/uapi
		${_LINUX_KERNEL_BUILD_DIR}/arch/x86/include/generated/uapi
		${_LINUX_KERNEL_BUILD_DIR}/include/uapi
		${_LINUX_KERNEL_BUILD_DIR}/include/generated/uapi
		${DEVMON_INC_DIR}
		${GEN_DIR}
	)

	add_definitions(
		-D__KERNEL__
		-DCC_USING_FENTRY
		-DMODULE
		-DKBUILD_BASENAME='"module"'
		-DKBUILD_MODNAME='"tdevmon"'
	)

	set(
		_LINUX_KERNEL_C_FLAGS
		-isystem /usr/lib/gcc/x86_64-pc-linux-gnu/${CMAKE_C_COMPILER_VERSION}/include
		-include ${_LINUX_KERNEL_BUILD_DIR}/include/linux/kconfig.h
		-include ${_LINUX_KERNEL_BUILD_DIR}/include/linux/compiler_types.h
		-nostdinc
		-fno-strict-aliasing
		-fno-common
		-fshort-wchar
		-fno-PIE
		-std=gnu89
		-mno-sse
		-mno-mmx
		-mno-sse2
		-mno-3dnow
		-mno-avx
		-fcf-protection=none
		-m64
		-falign-jumps=1
		-falign-loops=1
		-mno-80387
		-mno-fp-ret-in-387
		-mpreferred-stack-boundary=3
		-mskip-rax-setup
		-mtune=generic
		-mno-red-zone
		-mcmodel=kernel
		-fno-asynchronous-unwind-tables
		-mindirect-branch=thunk-extern
		-mindirect-branch-register
		-fno-jump-tables
		-fno-delete-null-pointer-checks
		-O2
		-fno-allow-store-data-races
		-fstack-protector-strong
		-g
		-gdwarf-4
		-pg
		-mrecord-mcount
		-mfentry
		-fno-strict-overflow
		-fno-stack-check
		-fconserve-stack
	)

	set(
		_LINUX_KERNEL_WARNING_FLAGS
		-Wall
		-Wundef
		-Wframe-larger-than=2048
		-Wimplicit-fallthrough
		-Wdeclaration-after-statement
		-Wvla
		-Werror=strict-prototypes
		-Werror=implicit-function-declaration
		-Werror=implicit-int
		-Werror=return-type
		-Werror=date-time
		-Werror=incompatible-pointer-types
		-Werror=designated-init
		-Wno-sign-compare
		-Wno-trigraphs
		-Wno-format-security
		-Wno-frame-address
		-Wno-format-truncation
		-Wno-format-overflow
		-Wno-address-of-packed-member
		-Wno-unused-but-set-variable
		-Wno-unused-const-variable
		-Wno-pointer-sign
		-Wno-stringop-truncation
		-Wno-zero-length-bounds
		-Wno-array-bounds
		-Wno-stringop-overflow
		-Wno-restrict
		-Wno-maybe-uninitialized
		-Wno-packed-not-aligned
	)

	set(
		EXTRA_C_FLAGS
		-fvisibility=hidden
		-Wno-date-time
		-Wno-parentheses
	)

	string(REPLACE ";" " " CMAKE_C_FLAGS "${_LINUX_KERNEL_C_FLAGS} ${_LINUX_KERNEL_WARNING_FLAGS} ${EXTRA_C_FLAGS}")

	add_library(
		dm_lnx_lkm
		${LKM_MAKEFILE_LIST}
		${LKM_PUBLIC_H_LIST}
		${LKM_H_LIST}
		${LKM_C_LIST}
		${GEN_H_LIST}
		${PCH_H}
		${PCH_C}
	)
endif()

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

install(
	FILES
	${LKM_H_LIST}
	${LKM_C_LIST}
	${GEN_H_LIST}
	DESTINATION ${DEVMON_INSTALL_LKM_SRC_SUBDIR}
	COMPONENT kernel
)

install(
	FILES
	${LKM_BUILD_DIR}/Makefile_install
	RENAME Makefile
	DESTINATION ${DEVMON_INSTALL_LKM_SRC_SUBDIR}
	COMPONENT kernel
)

install(
	FILES
	RENAME Makefile
	DESTINATION ${DEVMON_INSTALL_LKM_SRC_SUBDIR}
	COMPONENT kernel
)

#...............................................................................
