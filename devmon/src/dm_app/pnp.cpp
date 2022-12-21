#include "pch.h"
#include "pnp.h"
#include "error.h"
#include "dm_win_Service.h"

//..............................................................................

int
listClassDevices(const sl::StringRef& className) {
	bool result;

	char buffer[256];
	sl::Array<GUID> classGuidArray(rc::BufKind_Stack, buffer, sizeof(buffer));
	result = sys::win::DeviceInfoSet::getDeviceClassGuids(sl::String_w(className), &classGuidArray);
	if (!result) {
		printf("Can't get device class GUID for %s: %s\n", className.sz(), err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	printf("Devices of class %s:\n", className.sz());

	sys::win::DeviceInfoSet deviceInfoSet;
	result = deviceInfoSet.create(classGuidArray[0]);
	if (!result) {
		printf("Can't get list of devices of class %s: %s\n", className.sz(), err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	sys::win::DeviceInfo deviceInfo;
	sl::String string;

	size_t i;
	for (i = 0;; i++) {
		result = deviceInfoSet.getDeviceInfo(i, &deviceInfo);
		if (!result)
			break;

		result = deviceInfo.getDeviceRegistryProperty(SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, &string);
		if (!result) {
			printf("Can't get device name: %s\n", err::getLastErrorDescription().sz());
			return Error_IoFailure;
		}

		printf("%s", string.sz());

		result = deviceInfo.getDeviceRegistryProperty(SPDRP_FRIENDLYNAME, &string);
		if (result)
			printf(" %s", string.sz());

		printf("\n");
	}

	printf("Total devices: %d\n", i);
	return Error_Success;
}

int
listAllDevices() {
	bool result;

	sys::win::DeviceInfoSet deviceInfoSet;
	result = deviceInfoSet.create();
	if (!result) {
		printf("Can't get list of all devices: %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	sys::win::DeviceInfo deviceInfo;
	sl::String string;

	size_t i;
	for (i = 0;; i++) {
		result = deviceInfoSet.getDeviceInfo(i, &deviceInfo);
		if (!result)
			break;

		result = deviceInfo.getDeviceRegistryProperty(SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, &string);
		if (!result) {
			printf("Can't get device name: %s\n", err::getLastErrorDescription().sz());
			return Error_IoFailure;
		}

		printf("%s", string.sz());

		result = deviceInfo.getDeviceRegistryProperty(SPDRP_FRIENDLYNAME, &string);
		if (result)
			printf(" %s", string.sz());

		printf("\n");
	}

	printf("Total devices: %d\n", i);
	return Error_Success;
}

bool
findDeviceInfo(
	sys::win::DeviceInfoSet* deviceInfoSet,
	sys::win::DeviceInfo* deviceInfo,
	const sl::StringRef& deviceName
) {
	bool result;

	sys::win::DeviceInfo tmpDeviceInfo;
	sl::String string;

	size_t i;
	for (i = 0;; i++) {
		result = deviceInfoSet->getDeviceInfo(i, &tmpDeviceInfo);
		if (!result)
			break;

		result = tmpDeviceInfo.getDeviceRegistryProperty(SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, &string);
		if (!result)
			continue;

		if (_stricmp(string, deviceName.sz()) == 0) {
			*deviceInfo = tmpDeviceInfo;
			return true;
		}
	}

	return false;
}

void
prepareMultiSz(sl::String* string) {
	if (string->isEmpty())
		return;

	char* p = string->getBuffer();
	char* end = p + string->getLength() - 1;

	while (end > p && !*end)
		end--;

	string->overrideLength(end - p + 1);

	for (; p < end; p++)
		if (!*p)
			*p = ',';
}

int
printDeviceInfo(const sl::StringRef& deviceName) {
	bool result;

	sys::win::DeviceInfoSet deviceInfoSet;
	result = deviceInfoSet.create();
	if (!result) {
		printf("Can't get list of all devices: %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	sys::win::DeviceInfo deviceInfo;
	result = findDeviceInfo(&deviceInfoSet, &deviceInfo, deviceName);
	if (!result) {
		printf("Can't find device: %s\n", deviceName.sz());
		return Error_NameNotFount;
	}

	printf("Device name:   %s\n", deviceName.sz());

	sl::String string;

	result = deviceInfo.getDeviceRegistryProperty(SPDRP_HARDWAREID, &string);
	if (result) {
		prepareMultiSz(&string);
		printf("Hardware ID:   %s\n", string.sz());
	}

	result = deviceInfo.getDeviceRegistryProperty(SPDRP_FRIENDLYNAME, &string);
	if (result)
		printf("Friendly name: %s\n", string.sz());

	result = deviceInfo.getDeviceRegistryProperty(SPDRP_CLASS, &string);
	if (result)
		printf("Class:         %s\n", string.sz());

	result = deviceInfo.getDeviceRegistryProperty(SPDRP_CLASSGUID, &string);
	if (result)
		printf("Class GUID:    %s\n", string.sz());

	result = deviceInfo.getDeviceRegistryProperty(SPDRP_SERVICE, &string);
	if (result)
		printf("Service:       %s\n", string.sz());

	result = deviceInfo.getDeviceRegistryProperty(SPDRP_UPPERFILTERS, &string);
	if (result) {
		prepareMultiSz(&string);
		printf("Upper filters: %s\n", string.sz());
	}

	result = deviceInfo.getDeviceRegistryProperty(SPDRP_LOWERFILTERS, &string);
	if (result) {
		prepareMultiSz(&string);
		printf("Lower filters: %s\n", string.sz());
	}

	return Error_Success;
}

int
installPnpFilterForDeviceInfoSet(
	sys::win::DeviceInfoSet* deviceInfoSet,
	bool install
) {
	bool result;

	sys::win::DeviceInfo deviceInfo;
	sl::String physicalDeviceObjectName;
	sl::String friendlyName;

	const char* verbString = install ? "install" : "unininstall";
	bool isRebootRequired = false;

	size_t updatedDeviceCount = 0;
	for (size_t i = 0;; i++) {
		result = deviceInfoSet->getDeviceInfo(i, &deviceInfo);
		if (!result)
			break;

		result = deviceInfo.getDeviceRegistryProperty(SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, &physicalDeviceObjectName);
		if (!result)
			break;

		friendlyName.clear();
		result = deviceInfo.getDeviceRegistryProperty(SPDRP_FRIENDLYNAME, &friendlyName);
		if (result)
			friendlyName.append(' ');

		bool wasInstalled = dm::isPnpFilterInstalled(&deviceInfo);
		if (wasInstalled == install) {
			if (install) // don't show uninstall line
				printf("%s %s- already %sed\n", physicalDeviceObjectName.sz(), friendlyName.sz(), verbString);
			continue;
		}

		bool isDeviceRebootRequired;
		result = install ?
			dm::installPnpFilter(&deviceInfo, &isDeviceRebootRequired) :
			dm::uninstallPnpFilter(&deviceInfo, &isDeviceRebootRequired);

		if (!result) {
			printf(
				"%s %s- can't %s %s: %s\n",
				physicalDeviceObjectName.sz(),
				friendlyName.sz(),
				verbString,
				DM_PNP_SERVICE_NAMEA,
				err::getLastErrorDescription().sz()
			);

			return Error_IoFailure;
		}

		printf("%s %s- %sed", physicalDeviceObjectName.sz(), friendlyName.sz(), verbString);
		if (isDeviceRebootRequired) {
			printf("(reboot required)");
			isRebootRequired = true;
		}

		printf("\n");
		updatedDeviceCount++;
	}

	printf("Total devices updated: %d\n", updatedDeviceCount);

	if (isRebootRequired) {
		printf("Reboot required\n");
		return Error_RebootRequired;
	}

	return Error_Success;
}

int
installPnpFilterByClassName(
	const sl::StringRef& className,
	bool install
) {
	bool result;

	char buffer[256];
	sl::Array<GUID> classGuidArray(rc::BufKind_Stack, buffer, sizeof(buffer));
	result = sys::win::DeviceInfoSet::getDeviceClassGuids(sl::String_w(className), &classGuidArray);
	if (!result) {
		printf("Can't get device class GUID for %s: %s\n", className.sz(), err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	sys::win::DeviceInfoSet deviceInfoSet;
	result = deviceInfoSet.create(classGuidArray[0]);
	if (!result) {
		printf("Can't install %s for %s: %s\n", DM_PNP_SERVICE_NAMEA, className.sz(), err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	return installPnpFilterForDeviceInfoSet(&deviceInfoSet, install);
}

int
installPnpFilterByDeviceName(
	const sl::StringRef& deviceName0,
	bool install
) {
	bool result;

	sl::StringRef deviceName = deviceName0;

	sys::win::DeviceInfoSet deviceInfoSet;
	result = deviceInfoSet.create();
	if (!result) {
		printf("Can't get list of all devices: %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	sys::win::DeviceInfo deviceInfo;
	result = findDeviceInfo(&deviceInfoSet, &deviceInfo, deviceName);
	if (!result) {
		printf("Can't find device: %s\n", deviceName.sz());
		return Error_NameNotFount;
	}

	sl::String fullDeviceName;

	const char* verbString = install ? "install" : "unininstall";
	bool isRebootRequired = false;

	sl::String friendlyName;
	result = deviceInfo.getDeviceRegistryProperty(SPDRP_FRIENDLYNAME, &friendlyName);
	if (result) {
		fullDeviceName.format("%s %s", deviceName.sz(), friendlyName.sz());
		deviceName = fullDeviceName;
	}

	bool wasInstalled = dm::isPnpFilterInstalled(&deviceInfo);
	if (wasInstalled == install) {
		printf("%s - already %sed\n", deviceName.sz(), verbString);
		return Error_Success;
	}

	result = install ?
		dm::installPnpFilter(&deviceInfo, &isRebootRequired) :
		dm::uninstallPnpFilter(&deviceInfo, &isRebootRequired);

	if (!result) {
		printf(
			"%s - can't %s %s: %s\n",
			deviceName.sz(),
			verbString,
			DM_PNP_SERVICE_NAMEA,
			err::getLastErrorDescription().sz()
		);

		return Error_IoFailure;
	}

	printf("%s - %sed", deviceName.sz(), verbString);
	if (isRebootRequired) {
		printf("(reboot required)\n");
		return Error_RebootRequired;
	}

	printf("\n");
	return Error_Success;
}

int
installPnpFilter(
	const sl::StringRef& deviceOrClassName,
	bool install
) {
	static char deviceNamePrefix[] = "\\device\\";

	return _strnicmp(deviceOrClassName.sz(), deviceNamePrefix, lengthof(deviceNamePrefix)) == 0 ?
		installPnpFilterByDeviceName(deviceOrClassName, install) :
		installPnpFilterByClassName(deviceOrClassName, install);
}

int
removeAllPnpFilters() {
	printf("Removing all PNP filters...\n");

	sys::win::DeviceInfoSet deviceInfoSet;
	bool result = deviceInfoSet.create();
	if (!result) {
		printf("Can't get list of all devices: %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	return installPnpFilterForDeviceInfoSet(&deviceInfoSet, false);
}

//..............................................................................
