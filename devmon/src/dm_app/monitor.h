#pragma once

//..............................................................................

#if (_AXL_OS_WIN)

int
monitorDevice(
	const sl::StringRef& deviceName,
	const sl::StringRef& fileNameWildcard
);

int
monitorDevice_mi(
	const sl::StringRef& deviceName,
	const sl::StringRef& fileNameWildcard
);

#elif (_AXL_OS_LINUX)

int
monitorDevice(
	const sl::StringRef& deviceName,
	const sl::StringRef& fileNameWildcard,
	const struct dm_IoctlDesc* ioctlDescArray,
	size_t ioctlDescCount
);

int
monitorDevice_mi(
	const sl::StringRef& deviceName,
	const sl::StringRef& fileNameWildcard,
	const struct dm_IoctlDesc* ioctlDescArray,
	size_t ioctlDescCount
);

#endif

//..............................................................................
