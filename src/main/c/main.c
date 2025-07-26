#include <main.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <builder.h>
#include <tmplparser.h>
#include <confighandler.h>
#include <util/c_string.h>
#include <util/cliswitch.h>
#include <util/crossplatform.h>

#define PRINTLN(input) printf(input); printf("\n")
#define verbose(...) if (g_verbose) printf(__VA_ARGS__)

// returns `returnVal` if expr is an error value
#define try(expr, returnVal) if (isError(expr)) return returnVal

bool g_verbose;

void setVerbose(void) {
	g_verbose = true;
}

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
	PRINTLN("	--default      force uses the default template files");
	PRINTLN("	--help (-h)    prints this screen and exits");
	PRINTLN("	--version      prints the current version of CPM running");
	PRINTLN("	--verbose (-v) verbose output");

	exit(0);
}

void printVersion(void) {
	PRINTLN("CPM v2.2");

	exit(0);
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printHelp();
	}

	// light parsing step
	for (int i = 1; i < argc; i++) {
		if (strnlen(argv[i], 10) == 10) {
			continue;
		}

		if (strncmp(argv[i], "--verbose", 9) == 0) {
			g_verbose = true;
			continue;
		}

		if (strncmp(argv[i], "-v", 2) == 0) {
			g_verbose = true;
		}
	}

	verbose("cpm: adding switches..\n");

	// add switches
	try(addSwitch("-c",           setC)        , 1);
	try(addSwitch("--cpp",        setCPP)      , 1);
	try(addSwitch("--extended",   setExtended) , 1);
	try(addSwitch("--minimal",    setMinimal)  , 1);
	try(addSwitch("--no-folders", setNoFolders), 1);
	try(addSwitch("--default",    setDefault)  , 1);

	try(addSwitch("--help",       printHelp)   , 1);
	try(addSwitch("-h",           printHelp)   , 1);
	try(addSwitch("--version",    printVersion), 1);
	try(addSwitch("--verbose",    setVerbose)  , 1);
	try(addSwitch("-v",           setVerbose)  , 1);

	// change directory to load .tmpl files
	size_t pathMax = getPathMax();
	char* cwd = getCWD(pathMax);

	char* path = getExecutablePath(NULL);

	if (path == NULL) {
		printf("cpm: ERROR: failed to get current executable path\n");
		return 1;
	}

	changeWD(path);
	free(path);

	verbose("cpm: initializing config..\n");

	// initialize the config
	ProjectConfig config = config_init();
	config_makeCurrent(&config);

	config_setVerbose(g_verbose);

	verbose("cpm: parsing argv..\n");

	// full argv parsing step
	// important to call this *after* a config is current.
	int nonSwitchIndex = parseArgv(argc, argv);

	// no project names given or an invalid argument was given, so we'll quit
	// and print the help menu
	if (nonSwitchIndex == -1) {
		printHelp();
	}

	verbose("cpm: setting up config with parsed values..\n");

	config_setName(cstring_initFromConst(argv[nonSwitchIndex]));

	verbose("cpm: loading template files..\n");

	// load all necessary files
	config_loadFiles();
	if (isError(config_checkError())) {
		printf("cpm: ERROR: failed to create project\n");
		return 1;
	}

	changeWD(cwd);
	free(cwd);

	verbose("cpm: building project..\n");

	// build everything
	if (isError(buildProject())) {
		printf("cpm: ERROR: failed to create project\n");
		return 1;
	}

	verbose("cpm: finishing up..\n");
	printf("cpm: successfully created project '%s'\n", config_getCurrent().name.contents);
	config_freeCurrent();
}
