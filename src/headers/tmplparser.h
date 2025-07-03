#ifndef TMPL_PARSER
#define TMPL_PARSER

#include <main.h>

typedef struct {
	char* contents;
	size_t length;
} TMPLFile;

typedef struct {
	char* contents;
	size_t length;
} Line;

TMPLFile* tmpl_loadFile(const char* path);
char* tmpl_getContentsOfSection(TMPLFile* tmplFile, const char* sectionName, size_t* length);

#endif
