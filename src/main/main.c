#include "cliswitch.h"
#include <main.h>
#include <tmplparser.h>
#include <stdbool.h>

#ifdef LINUX
#include <unistd.h>
#include <linux/limits.h>
#endif

#ifdef WINDOWS
#include <windows.h>
#endif


typedef enum {
	EXTENDED,
	MINIMAL,
	NO_FOLDERS,
} Structure;

bool g_isC = true;
bool g_isCPP = false;
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
#endif
#ifdef WINDOWS
static char* getExecutablePath(size_t* len) {
	return NULL;
}
#endif

#ifndef LINUX
#ifndef WINDOWS

static char* getExecutablePath(size_t* len) {
	*len = 0; // pedantic errors really are pedantic aren't they

	printf("No platform set during compilation. Aborting..\n");
	exit(1);
}

#endif
#endif

static void changeWD(void) {
	size_t len;
	char* path = getExecutablePath(&len);

	if (path == NULL) {
		fprintf(stderr, "ERROR: failed to get executable path\n");
		exit(1);
	}

	int lastSlash = 0;
	for (size_t i = 0; i < len; i++) {
		if (path[i] == '/') {
			lastSlash = i;
		}
	}

	path[lastSlash + 1] = '\0';

	chdir(path);

	free(path);
}

void setC(void) {
	g_isC = true;
	g_isCPP = false;
}

void setCPP(void) {
	g_isC = false;
	g_isCPP = true;
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

int main(int argc, char** argv) {
	changeWD();

	TMPLFile* tmplFile = tmpl_loadFile("resources/templates-no-folders.tmpl");

	if (tmplFile == NULL) {
		printf("tmpl_loadFile error\n");
		return 1;
	}

	char* contents = tmpl_getContentsOfSection(tmplFile, "makefile", NULL);

	if (contents == NULL) {
		printf("tmpl_getContentsOfSection error\n");
		return 1;
	}

	//printf("contents: \n%s\n", contents);

	free(tmplFile);

	addSwitch("-c", setC);
	addSwitch("--cpp", setCPP);
	addSwitch("--extended", setExtended);
	addSwitch("--minimal", setMinimal);
	addSwitch("--no-folders", setNoFolders);

	int nonSwitchIndex = parseArgv(argc, argv);

	printf("current setup:\n c: %i, c++: %i\nproject structure: %i\n", g_isC, g_isCPP, g_projectStructure);
	printf("first nonswitch index: %i\n", nonSwitchIndex);
	if (nonSwitchIndex > 0) printf("first nonswitch argument: %s\n", argv[nonSwitchIndex]);
}
