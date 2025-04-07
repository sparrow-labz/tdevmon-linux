#...............................................................................
#
#  This file is part of Tibbo Device Monitor.
#  Copyright (c) 2014-2019, Tibbo Technology Inc
#  Author: Vladimir Gladkov
#
#  For legal details see accompanying license.txt file,
#  the public copy of which is also available at:
#  http://tibbo.com/downloads/archive/tdevmon/license.txt
#
#...............................................................................

if(WIN32)
	set(DEVMON_VERSION_MAJOR    3)
	set(DEVMON_VERSION_MINOR    3)
	set(DEVMON_VERSION_REVISION 7)
	set(DEVMON_VERSION_TAG      )
elseif(LINUX)
	set(DEVMON_VERSION_MAJOR    3)
	set(DEVMON_VERSION_MINOR    3)
	set(DEVMON_VERSION_REVISION 10)
	set(DEVMON_VERSION_TAG      )
else()
	set(DEVMON_VERSION_MAJOR    3)
	set(DEVMON_VERSION_MINOR    3)
	set(DEVMON_VERSION_REVISION 10)
	set(DEVMON_VERSION_TAG      )
endif()

set(DEVMON_VERSION_FULL "${DEVMON_VERSION_MAJOR}.${DEVMON_VERSION_MINOR}.${DEVMON_VERSION_REVISION}")

if(NOT "${DEVMON_VERSION_TAG}" STREQUAL "")
	set(DEVMON_VERSION_TAG_SUFFIX  " ${DEVMON_VERSION_TAG}")
	set(DEVMON_VERSION_FILE_SUFFIX "${DEVMON_VERSION_FULL}-${DEVMON_VERSION_TAG}")
else()
	set(DEVMON_VERSION_TAG_SUFFIX)
	set(DEVMON_VERSION_FILE_SUFFIX "${DEVMON_VERSION_FULL}")
endif()

string(TIMESTAMP DEVMON_VERSION_YEAR  "%Y")
string(TIMESTAMP DEVMON_VERSION_MONTH "%m")
string(TIMESTAMP DEVMON_VERSION_DAY   "%d")

set(DEVMON_VERSION_COMPANY "Tibbo Technology Inc")
set(DEVMON_VERSION_YEARS   "2007-${DEVMON_VERSION_YEAR}")

#...............................................................................
