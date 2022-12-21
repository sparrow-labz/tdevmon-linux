//..............................................................................

#include <axl_g_Pch.h>

#include <axl_sl_CmdLineParser.h>
#include <axl_sl_Singleton.h>
#include <axl_sys_Time.h>
#include <axl_sys_Thread.h>

#if (_AXL_OS_WIN)
#	include "dm_win_Pch.h"

#	include <axl_sys_win_AccessToken.h>
#	include <axl_sys_win_Sid.h>

#	include <lm.h>
#	include <io.h>
#	include <fcntl.h>
#elif (_AXL_OS_LINUX)
#	include "dm_lnx_Pch.h"

#	include <signal.h>
#endif

using namespace axl;

//..............................................................................

