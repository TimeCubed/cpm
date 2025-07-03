#include <main.h>
#include <tmplparser.h>
#include <unistd.h>
#include <linux/limits.h>

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
}

int main(void) {
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

	printf("contents: \n%s\n", contents);
}
