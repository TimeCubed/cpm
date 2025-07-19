#include "errors.h"
#include <main.h>
#include <string.h>
#include <builder.h>
#include <util/crossplatform.h>
#include <confighandler.h>

#define verbose(...) if (config_isCurrent()) if (config_getCurrent().verbose) printf(__VA_ARGS__)

static int createFile(const char* path, const String contents) {
	FILE* file = fopen(path, "w");
	
	if (file == NULL) {
		printf("builder: ERROR: failed to open file \'%s\'\n", path);

		return STATUS_FAIL;
	}

	size_t ret = fwrite(contents.contents, sizeof(char), contents.length, file);

	if (ret < contents.length) {
		printf("builder: ERROR: failed to write to file \'%s\'\n", path);

		fclose(file);
		return STATUS_FAIL;
	}

	fclose(file);
	return STATUS_OK;
}

static int createFiles(const char** fileNames, const String* fileContents, int fileCount) {
	verbose("builder: creating files..\n");

	for (int i = 0; i < fileCount; i++) {
		verbose("builder: creating file %s", fileNames[i]);

		if (createFile(fileNames[i], fileContents[i]) == STATUS_FAIL) {
			return STATUS_FAIL;
		}
	}

	return STATUS_OK;
}

static int setupExtendedStructure(ProjectConfig config) {
	if (makeDirectory(config.name.contents, 0755) == STATUS_FAIL) {
		return STATUS_FAIL;
	}

	changeWD(config.name.contents);

	char* subdirs[4] = {"src", "src/main/", "src/headers/", "src/resources/"};

	for (int i = 0; i < 4; i++) {
		if (makeDirectory(subdirs[i], 0755) == STATUS_FAIL) {
			return STATUS_FAIL;
		}
	}

	const char* files[3] = {"src/main/main.c", "src/headers/main.h", "makefile"};
	const String contents[3] = {config.mainC, config.mainH, config.makefile};

	return createFiles(files, contents, 3);
}

static int setupMinimalStructure(ProjectConfig config) {
	if (makeDirectory(config.name.contents, 0755) == STATUS_FAIL) {
		return STATUS_FAIL;
	}

	changeWD(config.name.contents);

	if (makeDirectory("src", 0755) == STATUS_FAIL) {
		return STATUS_FAIL;
	}

	const char* files[3] = {"src/main.c", "src/main.h", "makefile"};
	const String contents[3] = {config.mainC, config.mainH, config.makefile};

	return createFiles(files, contents, 3);
}

static int setupNoFoldersStructure(ProjectConfig config) {
	if (makeDirectory(config.name.contents, 0755) == STATUS_FAIL) {
		return STATUS_FAIL;
	}

	changeWD(config.name.contents);

	const char* files[3] = {"main.c", "main.h", "makefile"};
	const String contents[3] = {config.mainC, config.mainH, config.makefile};

	return createFiles(files, contents, 3);
}

int buildProject(void) {
	ProjectConfig config = config_getCurrent();
	if (config_checkError() == STATUS_FAIL) {
		printf("builder: ERROR: no config found\n");

		return STATUS_FAIL;
	}

	switch (config.projectStructure) {
		case EXTENDED:
			verbose("builder: setting up extended project structure..\n");
			return setupExtendedStructure(config);
		case MINIMAL:
			verbose("builder: setting up minimal project structure..\n");
			return setupMinimalStructure(config);
		case NO_FOLDERS:
			verbose("builder: setting up no-folders project structure..\n");
			return setupNoFoldersStructure(config);
	}

	return STATUS_FAIL;
}
