#pragma once

//..............................................................................

bool
isAdmin();

int
checkAccess();

int
printSecurityDescriptor();

int
printAccountInfo(const sl::StringRef& accountName);

int
listAllAccounts();

int
setAccess(
	const sl::ConstBoxList<sl::String>& denyList,
	const sl::ConstBoxList<sl::String>& allowList,
	int flags
);

//..............................................................................
