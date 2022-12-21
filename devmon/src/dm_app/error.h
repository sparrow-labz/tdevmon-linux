#pragma once

//..............................................................................

enum Error {
	Error_Success           = 0,
	Error_RebootRequired    = 1,
	Error_InvalidCmdLine    = -1,
	Error_IoFailure         = -2,
	Error_AdminRequired     = -3,
	Error_NameNotFount      = -4,
	Error_InvalidVersion    = -5,
};

//..............................................................................
