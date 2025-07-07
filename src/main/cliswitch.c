#include <main.h>
#include <cliswitch.h>
#include <stdbool.h>
#include <string.h>

Func* callbacks;
const char** switches;

size_t callbackC = 1, switchesC = 1;

static int init(void) {
	if (callbacks == NULL) {
		callbacks = calloc(callbackC, sizeof(Func));

		if (callbacks == NULL) {
			return STATUS_FAIL;
		}
	}

	if (switches == NULL) {
		switches = calloc(switchesC, sizeof(char**));

		if (switches == NULL) {
			return STATUS_FAIL;
		}
	}

	return STATUS_OK;
}

int addSwitch(const char* switchName, Func callback) {
	if (switchName == NULL || callback == NULL) {
		return STATUS_FAIL;
	}

	// no more than 32 characters allowed for a switch name
	if (strnlen(switchName, 32) == 32) {
		return STATUS_FAIL;
	}

	if (callbacks == NULL || switches == NULL) {
		if (init() == STATUS_FAIL) {
			return STATUS_FAIL;
		}


		callbacks[0] = callback;
		switches[0] = switchName;

		return STATUS_OK;
	}

	switchesC++;
	callbackC++;

	Func* tmp1 = realloc(callbacks, callbackC * sizeof(Func));
	const char** tmp2 = realloc(switches, switchesC * sizeof(char**));

	if (tmp1 == NULL || tmp2 == NULL) {
		free(callbacks);
		free(switches);

		return STATUS_FAIL;
	}

	callbacks = tmp1;
	switches = tmp2;

	callbacks[callbackC - 1] = callback;
	switches[switchesC - 1] = switchName;

	return STATUS_OK;
}

int parseArgv(int argc, char** argv) {
	bool foundNonSwitch = false;
	int nonSwitchIndex = -1;

	for (int i = 0; i < argc; i++) {
		if (strnlen(argv[i], 32) == 32) {
			continue;
		}

		bool isSwitch = false;
		for (size_t j = 0; j < switchesC; j++) {
			if (strncmp(argv[i], switches[j], 32) == 0) {
				callbacks[j]();

				isSwitch = true;
				break;
			}
		}

		if (!isSwitch && !foundNonSwitch && i != 0 && argv[i][0] != '-') {
			nonSwitchIndex = i;
			foundNonSwitch = true;
		}
	}

	return nonSwitchIndex;
}
