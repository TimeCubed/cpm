#ifndef TMPL_PARSER
#define TMPL_PARSER

#include <main.h>

typedef struct {
	const char* contents;
	size_t length;
} TMPLFile;

typedef struct {
	size_t length;
	size_t startIndex;
} Line;

TMPLFile* tmpl_loadFile(const char* path);
char* tmpl_getContentsOfSection(const TMPLFile* tmplFile, const char* sectionName, size_t* length);

#endif
