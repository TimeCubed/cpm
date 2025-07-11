#include <main.h>
#include <string.h>
#include <c_string.h>
#include <crossplatform.h>
#include <confighandler.h>

static int createFile(char* path, String contents) {
	FILE* file = fopen(path, "w");
	
	if (file == NULL) {
		printf("ERROR: failed to open file \'%s\'\n", path);

		return STATUS_FAIL;
	}

	size_t ret = fwrite(contents.contents, sizeof(char), contents.length, file);

	if (ret < contents.length) {
		printf("ERROR: failed to write to file \'%s\'\n", path);

		fclose(file);
		return STATUS_FAIL;
	}

	fclose(file);
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

	char* files[3] = {"src/main/main.c", "src/headers/main.h", "makefile"};
	String contents[3] = {config.mainC, config.mainH, config.makefile};

	for (int i = 0; i < 3; i++) {
		if (createFile(files[i], contents[i]) == STATUS_FAIL) {
			return STATUS_FAIL;
		}
	}

	return STATUS_OK;
}

static int setupMinimalStructure(ProjectConfig config) {
	if (makeDirectory(config.name.contents, 0755) == STATUS_FAIL) {
		return STATUS_FAIL;
	}

	changeWD(config.name.contents);

	if (makeDirectory("src", 0755) == STATUS_FAIL) {
		return STATUS_FAIL;
	}

	char* files[3] = {"src/main.c", "src/main.h", "makefile"};
	String contents[3] = {config.mainC, config.mainH, config.makefile};

	for (int i = 0; i < 3; i++) {
		if (createFile(files[i], contents[i]) == STATUS_FAIL) {
			return STATUS_FAIL;
		}
	}

	return STATUS_OK;
}

static int setupNoFoldersStructure(ProjectConfig config) {
	if (makeDirectory(config.name.contents, 0755) == STATUS_FAIL) {
		return STATUS_FAIL;
	}

	changeWD(config.name.contents);

	char* files[3] = {"main.c", "main.h", "makefile"};
	String contents[3] = {config.mainC, config.mainH, config.makefile};

	for (int i = 0; i < 3; i++) {
		if (createFile(files[i], contents[i]) == STATUS_FAIL) {
			return STATUS_FAIL;
		}
	}

	return STATUS_OK;
}

int buildProject(ProjectConfig config) {
	switch (config.projectStructure) {
		case EXTENDED:
			return setupExtendedStructure(config);
		case MINIMAL:
			return setupMinimalStructure(config);
		case NO_FOLDERS:
			return setupNoFoldersStructure(config);
	}

	return STATUS_FAIL;
}
