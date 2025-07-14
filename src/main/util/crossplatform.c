#include <main.h>
#include <string.h>
#include <c_string.h>
#include <crossplatform.h>

// ty stackoverflow
// https://stackoverflow.com/questions/3599160/how-can-i-suppress-unused-parameter-warnings-in-c
#define UNUSED(var) (void) (var)

#ifdef LINUX
size_t getPathMax(void) {
	return PATH_MAX;
}

char* getExecutablePath(size_t* len) {
	char* path = malloc(getPathMax());

	if (path == NULL) {
		return NULL;
	}

	size_t bufferLen = readlink("/proc/self/exe", path, PATH_MAX - 1);

	if (bufferLen == -1ul) {
		free(path);

		return NULL;
	}

	int lastSlash = 0;
	for (size_t i = 0; i < bufferLen; i++) {
		if (path[i] == '/') {
			lastSlash = i;
		}
	}

	if (lastSlash != (int) getPathMax()) {
		path[lastSlash + 1] = '\0';
	} else {
		path[getPathMax() - 1] = '\0';
	}

	if (len) *len = bufferLen;

	return path;
}

char* getCWD(size_t maxLength) {
	char* buf = malloc(maxLength);

	return getcwd(buf, maxLength);
}

int changeWD(char* path) {
	return chdir(path);
}

int makeDirectory(char* path, int mode) {
	if (mkdir(path, mode) != 0) {
		printf("ERROR: could not create directory \'%s\': %s\n", path, strerror(errno));
		return STATUS_FAIL;
	}

	return STATUS_OK;
}

char* getHomeDir(void) {
	return getenv("HOME");
}

char* getConfigDir(void) {
	return ".config/cpm/templates/";
}

char* translatePath(const char* path) {
	return (char*) path;
}
#endif

// TODO: implement all of this
// also, paths on windows are different than on linux
// C:\Users\user\ vs /home/user/
// be careful of that
#ifdef WINDOWS
char* getExecutablePath(size_t* len) {
	char* path = malloc(getPathMax());

	if (path == NULL) {
		return NULL;
	}

	// this doesn't include the null terminator, so we need to loop from i = 0
	// to i = bufferLen, NOT i = bufferLen - 1.
	size_t bufferLen = GetModuleFileNameA(NULL, path, getPathMax());

	int lastSlash = 0;
	for (size_t i = 0; i <= bufferLen; i++) {
		if (path[i] == '\\') {
			lastSlash = i;
		}
	}

	if (lastSlash != (int) getPathMax()) {
		path[lastSlash + 1] = '\0';
	} else {
		path[getPathMax() - 1] = '\0';
	}

	if (len) *len = bufferLen;

	return path;
}

size_t getPathMax(void) {
	return MAX_PATH;
}

char* getCWD(size_t maxLength) {
	char* buf = malloc(maxLength);

	// this is very bad, we don't check the return value of 
	// GetCurrentDirectory().
	//
	// again, just as in changeWD(), this is all because I'm implicitly
	// depending on the fact that getCWD returns a buffer, so I can't return an
	// error code for any error checking outside of this function.
	// the only other solution (other than refactoring of course) is to just
	// call exit(1) here, but that's really stupid, so I won't do that.
	GetCurrentDirectoryA(maxLength, buf);

	return buf;
}

int changeWD(char* path) {
	int ret = SetCurrentDirectoryA(path);

	// this is here because I probably wrote code that checks if changeWD was
	// successful by doing
	// if (changeWD(...) == 0)
	// instead of using the regular STATUS_FAIL/STATUS_OK stuff.
	// SetCurrentDirectory() returns a value > 0 for a successful operation, and
	// returns 0 if otherwise.
	//
	// TODO: probably should fix that
	if (ret > 0) {
		return 0;
	}

	return -1;
}

int makeDirectory(char* path, int _unused) {
	UNUSED(_unused);

	if (CreateDirectoryA(path, NULL) == 0) {
		DWORD error = GetLastError();

		if (error == ERROR_ALREADY_EXISTS) {
			printf("ERROR: could not create directory at '%s': directory already exists\n", path);

			return STATUS_FAIL;
		}

		if (error == ERROR_PATH_NOT_FOUND) {
			printf("ERROR: could not create directory at '%s': path not found\n", path);

			return STATUS_FAIL;
		}
	}

	return STATUS_OK;
}

char* getHomeDir(void) {
	return getenv("USERPROFILE");
}

char* getConfigDir(void) {
	return "AppData\\cpm\\templates\\";
}

char* translatePath(const char* path) {
	String pathString = cstring_initFromConst(path);
	size_t len = strnlen(pathString.contents, getPathMax());

	for (size_t i = 0; i < len; i++) {
		if (pathString.contents[i] == '/') {
			pathString.contents[i] = '\\';
		}
	}

	return pathString.contents;
}
#endif
