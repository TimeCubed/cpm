#include "errors.h"
#include <main.h>
#include <string.h>
#include <stdbool.h>
#include <builder.h>
#include <c_string.h>
#include <cliswitch.h>
#include <tmplparser.h>
#include <confighandler.h>
#include <crossplatform.h>

#define PRINTLN(input) printf(input); printf("\n")

bool g_isC = true;
Structure g_projectStructure = EXTENDED;

void setC(void) {
	g_isC = true;
}

void setCPP(void) {
	g_isC = false;
}

void setExtended(void) {
	g_projectStructure = EXTENDED;
}

void setMinimal(void) {
	g_projectStructure = MINIMAL;
}

void setNoFolders(void) {
	g_projectStructure = NO_FOLDERS;
}

void printHelp(void) {
	PRINTLN("CPM v2.0 ------------------------");
	PRINTLN("usage: cpm <project_name> [options]");
	PRINTLN(" ");
	PRINTLN("available options:");
	PRINTLN("	language options:");
	PRINTLN("	-c             sets the project's language as C (default)");
	PRINTLN("	--cpp          sets the project's language as C++");
	PRINTLN(" ");
	PRINTLN("	project structure settings:");
	PRINTLN("	--extended     uses the extended project structure (default)");
	PRINTLN("	--minimal      uses the minimal project structure");
	PRINTLN("	--no-folders   uses the no folders project structure");
	PRINTLN(" ");
	PRINTLN("	miscellaneous:");
	PRINTLN("	--help (-h)    prints this screen and exits");

	exit(0);
}


int main(int argc, char** argv) {
	// technically not required, as parseArgv will return -1 anyway, but it's
	// a small optimization to avoid allocating any ram for switch storage.
	// doesn't make it any less unnecessary, but my brain won't let me not do
	// this.
	if (argc < 2) {
		printHelp();
	}

	// add all switches and flags here
	addSwitch("-c",           setC);
	addSwitch("--cpp",        setCPP);
	addSwitch("--extended",   setExtended);
	addSwitch("--minimal",    setMinimal);
	addSwitch("--no-folders", setNoFolders);

	addSwitch("--help",       printHelp);
	addSwitch("-h",           printHelp);

	if (!g_isC) {
		printf("cpm: C++ is currently unsupported.\n");
		return 0;
	}

	int nonSwitchIndex = parseArgv(argc, argv);

	// no project names given, so we'll quit and print the help menu
	if (nonSwitchIndex == -1) {
		printHelp();
	}

	// before changing the current directory to load templates, save the current
	// working directory, so that we can come back later to create needed files.
	size_t pathMax = getPathMax();
	char* cwd = getCWD(pathMax);

	char* path = getExecutablePath(NULL);

	if (path == NULL) {
		printf("cpm: ERROR: failed to get current executable path\n");
		return 1;
	}

	changeWD(path);
	free(path);

	ProjectConfig config = config_init(
		cstring_init(argv[nonSwitchIndex], strnlen(argv[nonSwitchIndex], 64)),
		g_isC ? C : CPP,
		g_projectStructure
	);

	config_loadFiles(&config);

	if (config_checkError() == STATUS_FAIL) {
		printf("cpm: ERROR: failed to create project\n");
		return 1;
	}

	changeWD(cwd);
	free(cwd);

	if (buildProject(config) == STATUS_FAIL) {
		printf("cpm: ERROR: failed to create project\n");
		return 1;
	}

	config_free(config);
}
