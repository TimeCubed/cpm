#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

FILE* mainfile;
FILE* headerfile;
FILE* makefile;

int checkC(char* string) {
	return (strcmp(string, "c") == 0 || strcmp(string, "C") == 0);
}

int checkCpp(char* string) {
	return (strcmp(string, "cpp") == 0 || strcmp(string, "CPP") == 0 || strcmp(string, "c++") == 0);
}

void createCFile() {
	mainfile = fopen("main.c", "w");
	fputs("#include <stdio.h>\n#include <stdlib.h>\n\nint main(int argc, char **argv) {\n	printf(\"Hello, world!\");\n}", mainfile);
	headerfile = fopen("header.h", "w");
	fputs("#ifndef HEADER\n#define HEADER\n\n\n#endif", headerfile);
}

void createCppFile() {
	mainfile = fopen("main.cpp", "w");
	fputs("#include <iostream>\n\nusing namespace std;\n\nint main(int argc, char** argv) {\n	cout << \"Hello, world!\";\n}", mainfile);
	headerfile = fopen("header.hpp", "w");
	fputs("#ifndef HEADER\n#define HEADER\n\n\n#endif", headerfile);
}

void createMakeFile() {
	makefile = fopen("Makefile", "w");
	fputs("main:\n	gcc -o main main.c\nclean:\n	rm main", makefile);
}

void makeDirs(char* path, char** argv) {
	chdir(path);

	if (mkdir(argv[1], 0777) != 0) {
		// look, I know this looks really stupid, but idk how to do this any better
		char* error;
		asprintf(&error, "cpm: could not create project: could not create directory at \'%s\'", path);
		perror(error);
	}

	strcat(path, "/");
	strcat(path, argv[1]);

	chdir(path);
}

void createCFiles(char* path, char** argv) {
	makeDirs(path, argv);

	createCFile();
	createMakeFile();
}

void createCppFiles(char* path, char** argv) {
	makeDirs(path, argv);

	createCppFile();
	createMakeFile();
}

void printUsage(char* programName) {
	printf("usage: %s <project-name> [options]", programName);
	printf("\noptions:");
	printf("\n	c: creates a new C project (default behavior)");
	printf("\n	cpp/CPP/c++: creates a new C++ project");
}

int main(int argc, char** argv) {
	char* path = (char*) malloc(512);

	getcwd(path, 256);

	if (argc < 2) {
		printUsage(argv[0]);

		return 0;
	}

	if (strcmp(argv[1], "help") == 0) {
		printUsage(argv[0]);

		return 0;
	}

	if (argc < 3) {
		createCFiles(path, argv);

		return 0;
	}

	if (checkC(argv[2])) {
		createCFiles(path, argv);

		return 0;
	} else if (checkCpp(argv[2])) {
		createCppFiles(path, argv);

		return 0;
	}

	printUsage(argv[0]);
}
