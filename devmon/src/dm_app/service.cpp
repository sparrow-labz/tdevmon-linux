#include "pch.h"
#include "service.h"
#include "pnp.h"
#include "error.h"

//..............................................................................

int
createService(dm::ServiceKind serviceKind) {
	const wchar_t* serviceName = getServiceName(serviceKind);

	if (dm::isServiceCreated(serviceKind)) {
		printf("%S is already created\n", serviceName);
		return Error_Success;
	}

	bool result = dm::createService(serviceKind);
	if (!result) {
		printf("Can't create %S: %s\n", serviceName, err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	printf("%S created\n", serviceName);
	return Error_Success;
}

int
deleteService(dm::ServiceKind serviceKind) {
	const wchar_t* serviceName = getServiceName(serviceKind);

	if (!dm::isServiceCreated(serviceKind)) {
		printf("%S is not created\n", serviceName);
		return Error_Success;
	}

	bool result = dm::deleteService(serviceKind);
	if (!result) {
		printf("Can't delete %S: %s\n", serviceName, err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	printf("%S deleted\n", serviceName);
	return Error_Success;
}

int
startCoreService() {
	bool result;

	if (dm::isCoreServiceStarted()) {
		printf(DM_CORE_SERVICE_NAMEA " is already started\n");
		return Error_Success;
	}

	if (!dm::isServiceCreated(dm::ServiceKind_Core)) {
		result = dm::createService(dm::ServiceKind_Core);
		if (!result) {
			printf("Can't create " DM_CORE_SERVICE_NAMEA ": %s\n", err::getLastErrorDescription().sz());
			return Error_IoFailure;
		}

		printf(DM_CORE_SERVICE_NAMEA " created\n");
	}

	result = dm::startCoreService();
	if (!result) {
		printf("Can't start " DM_CORE_SERVICE_NAMEA ": %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	printf(DM_CORE_SERVICE_NAMEA " started\n");
	return Error_Success;
}

int
stopCoreService(bool isForced) {
	if (!dm::isCoreServiceStarted()) {
		printf(DM_CORE_SERVICE_NAMEA " is not started\n");
		return Error_Success;
	}

	bool isRebootRequired = false;
	bool result = isForced ?
		dm::forceStopCoreService(&isRebootRequired) :
		dm::stopCoreService();

	if (!result) {
		printf("Can't stop " DM_CORE_SERVICE_NAMEA ": %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	if (isRebootRequired) {
		printf(DM_CORE_SERVICE_NAMEA " stopped (reboot required)\n");
		return Error_RebootRequired;
	}

	printf(DM_CORE_SERVICE_NAMEA " stopped\n");
	return Error_Success;
}

int
restartCoreService(bool isForced) {
	bool result = dm::isCoreServiceStarted();
	if (!result)
		return startCoreService();

	bool isRebootRequired = false;
	result = isForced ?
		dm::forceStopCoreService(&isRebootRequired) :
		dm::stopCoreService();

	if (!result) {
		printf("Can't stop " DM_CORE_SERVICE_NAMEA ": %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	result = dm::startCoreService();
	if (!result) {
		printf("Can't start " DM_CORE_SERVICE_NAMEA ": %s\n", err::getLastErrorDescription().sz());
		return Error_IoFailure;
	}

	if (isRebootRequired) {
		printf(DM_CORE_SERVICE_NAMEA " restarted (reboot required)\n");
		return Error_RebootRequired;
	}

	printf(DM_CORE_SERVICE_NAMEA " restarted\n");
	return Error_Success;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

int
install() {
	int result;
	int finalResult;

	result = ::createService(dm::ServiceKind_Pnp);
	if (result < 0)
		return result;

	finalResult = result;

	result = startCoreService();
	if (result < 0)
		return result;

	finalResult = AXL_MAX(finalResult, result);

	return finalResult;
}

int
uninstall(bool isForced) {
	int result;
	int finalResult;

	result = removeAllPnpFilters();
	if (result < 0)
		return result;

	finalResult = result;

	result = stopCoreService(isForced);
	if (result < 0)
		return result;

	finalResult = AXL_MAX(finalResult, result);

	result = ::deleteService(dm::ServiceKind_Core);
	if (result < 0)
		return result;

	finalResult = AXL_MAX(finalResult, result);

	result = ::deleteService(dm::ServiceKind_Pnp);
	if (result < 0)
		return result;

	finalResult = AXL_MAX(finalResult, result);

	return finalResult;
}

int
reinstall(bool isForced) {
	int result;
	int finalResult;

	// TODO: restart filtered devices instead of removing all filters

	result = removeAllPnpFilters();
	if (result < 0)
		return result;

	finalResult = result;

	result = stopCoreService(isForced);
	if (result < 0)
		return result;

	finalResult = AXL_MAX(finalResult, result);

	result = ::createService(dm::ServiceKind_Pnp);
	if (result < 0)
		return result;

	finalResult = result;

	result = startCoreService();
	if (result < 0)
		return result;

	finalResult = AXL_MAX(finalResult, result);

	return finalResult;
}

//..............................................................................
