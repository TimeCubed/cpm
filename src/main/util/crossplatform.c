#include <main.h>
#include <errno.h>
#include <string.h>
#include <crossplatform.h>

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
#endif

// TODO: implement all of this
// also, paths on windows are different than on linux
// C:\Users\user\ vs /home/user/
// be careful of that
#ifdef WINDOWS
char* getExecutablePath(size_t* len) {
	return NULL;
}

size_t getPathMax(void) {
	return MAX_PATH;
}

char* getCWD(size_t maxLength) {
	char* buf = malloc(maxLength);

	return _getcwd(buf, maxLength);
}

int changeWD(char* path) {
	return SetCurrentDirectory(path);
}

int makeDirectory(char* path, int _unused) {
	return _mkdir(path);
}
#endif
