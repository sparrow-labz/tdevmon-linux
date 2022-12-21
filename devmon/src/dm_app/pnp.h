#pragma once

//..............................................................................

int
listClassDevices(const sl::StringRef& className);

int
listAllDevices();

int
installPnpFilterForDeviceInfoSet(
	sys::win::DeviceInfoSet* deviceInfoSet,
	bool install
);

int
installPnpFilterByClassName(
	const sl::StringRef& className,
	bool install
);

int
installPnpFilterByDeviceName(
	const sl::StringRef& deviceName,
	bool install
);

int
installPnpFilter(
	const sl::StringRef& deviceOrClassName,
	bool install
);

int
removeAllPnpFilters();

int
printDeviceInfo(const sl::StringRef& deviceName);

//..............................................................................
