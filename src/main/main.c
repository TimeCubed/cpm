#include <main.h>
#include <tmplparser.h>
#include <cliswitch.h>
#include <stdbool.h>

#ifdef LINUX
#include <unistd.h>
#include <linux/limits.h>
#endif

#ifdef WINDOWS
#include <windows.h>
#endif

#define PRINTLN(input) printf((char*) input); printf("\n")

typedef enum {
	EXTENDED,
	MINIMAL,
	NO_FOLDERS,
} Structure;

bool g_isC = true;
Structure g_projectStructure = EXTENDED;

#ifdef LINUX
static char* getExecutablePath(size_t* len) {
	char* buffer = malloc(PATH_MAX);

	*len = readlink("/proc/self/exe", buffer, PATH_MAX - 1);

	if (*len == -1ul) {
		fprintf(stderr, "ERROR: couldn't get executable path\n");

		free(buffer);

		return NULL;
	}

	buffer[*len - 1] = '\0';

	return buffer;
}

static size_t getPathMax(void) {
	return PATH_MAX;
}

static char* getCWD(size_t maxLength) {
	char* buf = malloc(maxLength);

	return getcwd(buf, maxLength);
}

static int changeWD(char* path) {
	return chdir(path);
}
#endif

// TODO: implement all of this
// also, paths on windows are different than on linux
// C:\Users\user\ vs /home/user/
// be careful of that
#ifdef WINDOWS
static char* getExecutablePath(size_t* len) {
	return NULL;
}

static size_t getPathMax(void) {
	return MAX_PATH;
}

static char* getCWD(size_t maxLength) {
	char* buf = malloc(maxLength);

	return _getcwd(buf, maxLength);
}

static int changeWD(char* path) {
	return SetCurrentDirectory(path);
}
#endif


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

void setupExtendedStructure(void) {

}

void setupMinimalStructure(void) {

}

void setupNoFoldersStructure(void) {

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

	size_t len;
	char* path = getExecutablePath(&len);

	if (path == NULL) {
		fprintf(stderr, "cpm: ERROR: failed to get current executable path\n");
		return 1;
	}

	int lastSlash = 0;
	for (size_t i = 0; i < len; i++) {
		if (path[i] == '/' || path[i] == '\\') {
			lastSlash = i;
		}
	}

	path[lastSlash + 1] = '\0';

	changeWD(path);
	free(path);

	TMPLFile* tmplFile;

	switch (g_projectStructure) {
		case EXTENDED:
			tmplFile = tmpl_loadFile("resources/templates-extended-c.tmpl");

			if (tmplFile == NULL) {
				printf("cpm: ERROR: failed to load templates for extended project structure\n");
				return 1;
			}

			break;
		case MINIMAL:
			tmplFile = tmpl_loadFile("resources/templates-minimal-c.tmpl");

			if (tmplFile == NULL) {
				printf("cpm: ERROR: failed to load templates for minimal project structure\n");
				return 1;
			}

			break;
		case NO_FOLDERS:
			tmplFile = tmpl_loadFile("resources/templates-no-folders-c.tmpl");

			if (tmplFile == NULL) {
				printf("cpm: ERROR: failed to load templates for no-folders project structure\n");
				return 1;
			}

			break;
		default:
			printf("cpm: ERROR: unknown error occured\n");
			return -1;
	}

	// TODO: add c++ templates

	size_t mcLength, mhLength, mkLength;
	char* mainC    = tmpl_getContentsOfSection(tmplFile, "main.c", &mcLength);
	char* mainH    = tmpl_getContentsOfSection(tmplFile, "main.h", &mhLength);
	char* makefile = tmpl_getContentsOfSection(tmplFile, "makefile", &mkLength);

	if (mainC == NULL || mainH == NULL || makefile == NULL) {
		printf("cpm: ERROR: failed to read from template files\n");
		return 1;
	}

	free(tmplFile);
	
	changeWD(cwd);
	free(cwd);

	printf("%s\n%s\n%s\n", mainC, mainH, makefile);

	// switches look so awful oh my god
	if (g_isC) {
		switch (g_projectStructure) {
			case EXTENDED:
				setupExtendedStructure();
				break;
			case MINIMAL:
				setupMinimalStructure();
				break;
			case NO_FOLDERS:
				setupNoFoldersStructure();
				break;
		}
	}
}
