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

/**
 * Loads a .tmpl file from `path`.
 *
 * @param path The path of the file to load.
 *
 * @return `STATUS_OK` if successful, `STATUS_FAIL` otherwise.
 */
TMPLFile* tmpl_loadFile(const char* path);

/**
 * Searches inside the contents of the provided .tmpl file for a section with
 * the name `sectionName` and grabs the contents of that section.
 *
 * @param tmplFile The .tmpl file.
 * @param sectionName The name of the section.
 * @param length The int pointed to by this parameter is set to the length of
 *               the section's contents.
 *
 * @return The contents of the section requested, `NULL` if not found.
 */
char* tmpl_getContentsOfSection(const TMPLFile* tmplFile, const char* sectionName, size_t* length);

#endif
