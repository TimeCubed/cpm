#ifndef TMPL_PARSER
#define TMPL_PARSER

#include <main.h>
#include <stdbool.h>

typedef struct {
	char* contents;
	size_t length;
} TMPLFile;

typedef struct {
	size_t length;
	size_t startIndex;
} Line;

// error handling struct that is a shameless copy of C++23's std::expected
typedef struct {
	union {
		TMPLFile* tmplFile;
		const char* errorMessage;
	};

	bool hasValue;
} LoaderStatus;

// this is why c++ has templates
typedef struct {
	union {
		char* sectionContents;
		const char* errorMessage;
	};

	bool hasValue;
} ParserStatus;

/**
 * Loads a .tmpl file from `path`.
 * 
 * Note: The loaded contents of the file is automatically terminated by a null
 * byte. The length of the file contents is set as the number of bytes in the 
 * loaded file + 1 (i.e. including the null byte).
 *
 * @param path The path of the file to load.
 *
 * @return A `LoaderStatus` object, indicating the status of the operation.
 *         If the object's `hasValue` field is set to `true`, that indicates
 *         that the operation was successful, and you may access the object's
 *         `tmplFile` field to obtain a pointer to a heap-allocated `TMPLFile`
 *         object. If `hasValue` is `false`, that indicates that an error
 *         occured, and you may access `errorMessage` to get more details on the
 *         error.
 *
 *         Note: The `tmplFile` and `errorMessage` fields are wrapped in a
 *         union, meaning that if one is set, the other is pointing to invalid
 *         data.
 *         The caller is expected to call `tmpl_free()` on the returned object.
 */
LoaderStatus tmpl_loadFile(const char* path);

/**
 * Searches inside the contents of the provided .tmpl file for a section with
 * the name `sectionName` and grabs the contents of that section.
 *
 * @param tmplFile The .tmpl file.
 * @param sectionName The name of the section.
 * @param length The int pointed to by this parameter is set to the length of
 *               the section's contents.
 *
 * @return A `ParserStatus` object, indicating the status of the operation.
 *         If the object's `hasValue` field is set to `true`, that indicates
 *         that the operation was successful, and you may acces the object's
 *         `sectionContents` field to obtain a pointer to the contents of the
 *         requested section. If `hasValue` is false, that indicates that an
 *         error occurred, and you may access `errorMessage` to get more details
 *         on the error.
 *
 *         Note : The `sectionContents` and `errorMessage` fields are wrapped in
 *         a union, meanig that if one is set, the other is likely pointing to
 *         invalid data.
 */
ParserStatus tmpl_getContentsOfSection(const TMPLFile* tmplFile, const char* sectionName, size_t* length);

/**
 * Frees all data associated with the given `TMPLFile` object.
 */
void tmpl_free(TMPLFile* tmplFile);

#endif
