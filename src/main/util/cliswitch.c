#include <main.h>
#include <string.h>
#include <stdbool.h>
#include <util/cliswitch.h>

#define MAX_SWITCH_NAME_LEN 32

Func* callbacks;
const char** switches;

size_t callbackC = 1, switchesC = 1;

static int init(void) {
	if (callbacks == NULL) {
		callbacks = calloc(callbackC, sizeof(Func));

		if (callbacks == NULL) {
			return ERROR_NOT_ENOUGH_MEMORY;
		}
	}

	if (switches == NULL) {
		switches = calloc(switchesC, sizeof(char**));

		if (switches == NULL) {
			return ERROR_NOT_ENOUGH_MEMORY;
		}
	}

	return STATUS_OK;
}

int addSwitch(const char* switchName, Func callback) {
	if (switchName == NULL || callback == NULL) {
		return STATUS_FAIL;
	}

	// no more than 32 characters allowed for a switch name
	if (strnlen(switchName, MAX_SWITCH_NAME_LEN) == MAX_SWITCH_NAME_LEN) {
		return STATUS_FAIL;
	}

	if (callbacks == NULL || switches == NULL) {
		if (isError(init())) {
			return ERROR_NOT_ENOUGH_MEMORY;
		}


		callbacks[0] = callback;
		switches[0] = switchName;

		return STATUS_OK;
	}

	switchesC++;
	callbackC++;

	Func* tmp1 = realloc(callbacks, callbackC * sizeof(Func));

	if (tmp1 == NULL) {
		free(switches);
		free(callbacks);

		return ERROR_NOT_ENOUGH_MEMORY;
	}

	callbacks = tmp1;

	const char** tmp2 = realloc(switches, switchesC * sizeof(char**));

	if (tmp2 == NULL) {
		free(switches);
		free(callbacks);

		return ERROR_NOT_ENOUGH_MEMORY;
	}

	switches = tmp2;

	callbacks[callbackC - 1] = callback;
	switches[switchesC - 1] = switchName;

	return STATUS_OK;
}

int parseArgv(int argc, char** argv) {
	bool foundNonSwitch = false;
	int nonSwitchIndex = -1;

	for (int i = 1; i < argc; i++) {
		if (strnlen(argv[i], MAX_SWITCH_NAME_LEN) == MAX_SWITCH_NAME_LEN) {
			printf("WARN: argument %s is longer than %i characters, skipping\n", argv[i], MAX_SWITCH_NAME_LEN);
			continue;
		}

		bool isSwitch = false;
		for (size_t j = 0; j < switchesC; j++) {
			if (strncmp(argv[i], switches[j], MAX_SWITCH_NAME_LEN) == 0) {
				callbacks[j]();

				isSwitch = true;
				break;
			}
		}

		// non-switch, doesn't begin with '-' -> first non-switch argument
		if (!isSwitch && !foundNonSwitch && argv[i][0] != '-') {
			nonSwitchIndex = i;
			foundNonSwitch = true;

			continue;
		}

		// invalid switch
		if (!isSwitch && argv[i][0] == '-') {
			printf("cpm: ERROR: invalid argument given at position %i: %s\n", i, argv[i]);
			return -1;
		}

		if (!isSwitch && foundNonSwitch) {
			return -1;
		}
	}

	return nonSwitchIndex;
}
