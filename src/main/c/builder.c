#include <main.h>
#include <string.h>
#include <assert.h>
#include <builder.h>
#include <util/crossplatform.h>
#include <confighandler.h>

#define verbose(...) if (config_isCurrent()) if (config_getCurrent().verbose) printf(__VA_ARGS__)

static error_t createFile(const char* path, const String contents) {
	FILE* file = fopen(path, "w");
	
	if (file == NULL) {
		printf("builder: ERROR: failed to open file \'%s\'\n", path);

		return ERROR_FILE_OPEN_FAIL;
	}

	size_t ret = fwrite(contents.contents, sizeof(char), contents.length, file);

	if (ret < contents.length) {
		printf("builder: ERROR: failed to write to file \'%s\'\n", path);

		fclose(file);
		return ERROR_FILE_WRITE_FAIL;
	}

	fclose(file);
	return STATUS_OK;
}

static error_t createFiles(const char** fileNames, const String* fileContents, int fileCount) {
	verbose("builder: creating files..\n");

	for (int i = 0; i < fileCount; i++) {
		verbose("builder: creating file %s\n", fileNames[i]);

		if (isError(createFile(fileNames[i], fileContents[i]))) {
			return STATUS_FAIL;
		}
	}

	return STATUS_OK;
}

static int setupExtendedStructure(ProjectConfig config) {
	if (isError(makeDirectory(config.name.contents, 0755))) {
		return STATUS_FAIL;
	}

	changeWD(config.name.contents);

	char* subdirs[4] = {"src", "src/main/", "src/headers/", "src/resources/"};

	for (int i = 0; i < 4; i++) {
		if (isError(makeDirectory(subdirs[i], 0755))) {
			return STATUS_FAIL;
		}
	}

	const char* files[3] = {"src/main/main.c", "src/headers/main.h", "makefile"};

	// I hate having to hardcode these, but it'll make do.
	// more motivation to get this finished and start work on v3, I guess.
	if (config.language == CPP) {
		files[0] = "src/main/main.cpp";
		files[1] = "src/headers/main.hpp";
	}

	const String contents[3] = {config.mainC, config.mainH, config.makefile};

	return createFiles(files, contents, 3);
}

static int setupMinimalStructure(ProjectConfig config) {
	if (isError(makeDirectory(config.name.contents, 0755))) {
		return STATUS_FAIL;
	}

	changeWD(config.name.contents);

	if (isError(makeDirectory("src", 0755))) {
		return STATUS_FAIL;
	}

	const char* files[3] = {"src/main.c", "src/main.h", "makefile"};

	// I hate having to hardcode these, but it'll make do.
	// more motivation to get this finished and start work on v3, I guess.
	if (config.language == CPP) {
		files[0] = "src/main.cpp";
		files[1] = "src/main.hpp";
	}

	const String contents[3] = {config.mainC, config.mainH, config.makefile};

	return createFiles(files, contents, 3);
}

static int setupNoFoldersStructure(ProjectConfig config) {
	if (isError(makeDirectory(config.name.contents, 0755))) {
		return STATUS_FAIL;
	}

	changeWD(config.name.contents);

	const char* files[3] = {"main.c", "main.h", "makefile"};

	// I hate having to hardcode these, but it'll make do.
	// more motivation to get this finished and start work on v3, I guess.
	//
	// did you like the 3 comments about this one thing?
	// yeah, I *really* don't like having to hardcode everything. I swear to god
	// the only reason I'm finishing this up at all is so I can get to work on
	// v3, this sucks ass to do
	if (config.language == CPP) {
		files[0] = "main.cpp";
		files[1] = "main.hpp";
	}
	
	const String contents[3] = {config.mainC, config.mainH, config.makefile};

	return createFiles(files, contents, 3);
}

error_t buildProject(void) {
	ProjectConfig config = config_getCurrent();
	if (config_checkError() == STATUS_FAIL) {
		printf("builder: ERROR: no config found\n");

		return ERROR_NO_CURRENT_CONFIG;
	}

	// it's likely that we can guarantee that config.language is either C or CPP at this point, but
	// who knows? maybe a solar ray will bless someone's PC with flipping a bit in a single register
	// at the right moment to mess everything up.
	assert(config.language == C || config.language == CPP);

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
