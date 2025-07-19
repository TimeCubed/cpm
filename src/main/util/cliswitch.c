#include <main.h>
#include <string.h>
#include <stdbool.h>
#include <util/cliswitch.h>

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

	if (tmp1 == NULL) {
		free(callbacks);

		return STATUS_FAIL;
	}

	const char** tmp2 = realloc(switches, switchesC * sizeof(char**));

	if (tmp2 == NULL) {
		if (tmp1 != NULL) free(tmp1);
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

	for (int i = 1; i < argc; i++) {
		if (strnlen(argv[i], 32) == 32) {
			printf("cpm: WARN: argument %s is longer than 32 characters, skipping\n", argv[i]);
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

		// second project name
		if (!isSwitch && foundNonSwitch) {
			printf("cpm: ERROR: multiple project names given\n");
			return -1;
		}
	}

	return nonSwitchIndex;
}
