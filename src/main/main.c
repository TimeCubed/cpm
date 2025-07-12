#include <main.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <builder.h>
#include <c_string.h>
#include <cliswitch.h>
#include <tmplparser.h>
#include <confighandler.h>
#include <crossplatform.h>

#define PRINTLN(input) printf(input); printf("\n")

bool g_isC = true, g_defaultTemplates = false;
Structure g_projectStructure = EXTENDED;

void setC(void) {
	assert(config_isCurrent());
	config_setLanguage(C);
}

void setCPP(void) {
	assert(config_isCurrent());
	config_setLanguage(CPP);
}

void setExtended(void) {
	assert(config_isCurrent());
	config_setStructure(EXTENDED);
}

void setMinimal(void) {
	assert(config_isCurrent());
	config_setStructure(MINIMAL);
}

void setNoFolders(void) {
	assert(config_isCurrent());
	config_setStructure(NO_FOLDERS);
}

void setDefault(void) {
	assert(config_isCurrent());
	config_setDefaultTemplates(true);
}

void printHelp(void) {
	PRINTLN("usage: cpm <project_name> [options]");
	printf("\n");
	PRINTLN("available options:");
	PRINTLN("	language options:");
	PRINTLN("	-c             sets the project's language as C (default)");
	PRINTLN("	--cpp          sets the project's language as C++");
	printf("\n");
	PRINTLN("	project structure settings:");
	PRINTLN("	--extended     uses the extended project structure (default)");
	PRINTLN("	--minimal      uses the minimal project structure");
	PRINTLN("	--no-folders   uses the no folders project structure");
	printf("\n");
	PRINTLN("	miscellaneous:");
	PRINTLN("	--help (-h)    prints this screen and exits");
	PRINTLN("	--version (-v) prints the current version of CPM running");

	exit(0);
}

void printVersion(void) {
	PRINTLN("CPM v2.0");

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
	addSwitch("--default",    setDefault);

	addSwitch("--help",       printHelp);
	addSwitch("-h",           printHelp);
	addSwitch("--version",    printVersion);
	addSwitch("-v",           printVersion);

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

	ProjectConfig config = config_init();
	config_makeCurrent(&config);

	// important to call this *after* a config is current.
	int nonSwitchIndex = parseArgv(argc, argv);

	// no project names given, so we'll quit and print the help menu
	if (nonSwitchIndex == -1) {
		printHelp();
	}

	config_setName(cstring_init(
		argv[nonSwitchIndex], strnlen(argv[nonSwitchIndex], 32)
	));

	config_loadFiles();
	if (config_checkError() == STATUS_FAIL) {
		printf("cpm: ERROR: failed to create project\n");
		return 1;
	}

	changeWD(cwd);
	free(cwd);

	if (buildProject() == STATUS_FAIL) {
		printf("cpm: ERROR: failed to create project\n");
		return 1;
	}

	config_freeCurrent();
}
