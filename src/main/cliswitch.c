#include <main.h>
#include <cliswitch.h>
#include <string.h>

Func* callbacks;
const char** switches;

size_t callbackC = 1, switchesC = 1;

static int init(void) {
	if (callbacks == NULL) {
		callbacks = calloc(1, sizeof(Func));

		if (callbacks == NULL) {
			return STATUS_FAIL;
		}
	}

	if (switches == NULL) {
		switches = calloc(1, sizeof(char**));

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

	callbacks = realloc(callbacks, callbackC);
	switches = realloc(switches, switchesC);

	if (callbacks == NULL || switches == NULL) {
		return STATUS_FAIL;
	}

	callbacks[callbackC - 1] = callback;
	switches[switchesC - 1] = switchName;

	return STATUS_OK;
}

void parseArgv(int argc, char** argv) {
	for (int i = 0; i < argc; i++) {
		if (strnlen(argv[i], 32) == 32) {
			continue;
		}

		for (size_t j = 0; j < switchesC; j++) {
			if (strncmp(argv[i], switches[j], 32) == 0) {
				callbacks[j]();
				break;
			}
		}
	}
}
