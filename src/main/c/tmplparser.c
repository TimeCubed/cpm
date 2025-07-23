#include <main.h>
#include <string.h>
#include <stdbool.h>
#include <tmplparser.h>
#include <confighandler.h>

#define verbose(...) if (config_isCurrent()) if (config_getCurrent().verbose) printf(__VA_ARGS__)

#define READ_CHUNK 128

typedef struct {
	size_t length;
	size_t startIndex;
} Line;

LoaderStatus tmpl_loadFile(const char* path) {
	LoaderStatus status;
	TMPLFile* tmplFile = calloc(1, sizeof(TMPLFile));

	if (tmplFile == NULL) {
		status = (LoaderStatus) {
			.errorMessage = "ERROR: not enough memory\n",
			.hasValue = false
		};

		return status;
	}

	FILE* file = fopen(path, "r");

	if (file == NULL) {
		free(tmplFile);

		status = (LoaderStatus) {
			.errorMessage = "ERROR: could not open file for parsing\n",
			.hasValue = false
		};

		return status;
	}

	size_t used = 0, bufSize = READ_CHUNK;
	char* contents = malloc(READ_CHUNK);

	if (contents == NULL) {
		free(tmplFile);
		fclose(file);

		status = (LoaderStatus) {
			.errorMessage = "ERROR: not enough memory\n",
			.hasValue = false
		};

		return status;
	}

	while (1) {
		if (used + READ_CHUNK + 1 > bufSize) {
			bufSize += READ_CHUNK + 1;

			char* tmp = realloc(contents, bufSize);

			if (tmp == NULL) {
				free(contents);
				free(tmplFile);
				fclose(file);

				status = (LoaderStatus) {
					.errorMessage = "ERROR: not enough memory\n",
					.hasValue = false
				};

				return status;
			}

			contents = tmp;
		}

		size_t ret = fread(contents + used, sizeof(char), READ_CHUNK, file);

		if (ret == 0) {
			break;
		}

		used += ret;
	}

	contents[used] = '\0';

	tmplFile->length = used;
	tmplFile->contents = contents;

	fclose(file);

	status = (LoaderStatus) {
		.tmplFile = tmplFile,
		.hasValue = true
	};

	return status;
}

/**
 * Splits the given string on each newline character, returning a heap-allocated
 * array of `Line` structs. If the string does not end in a '\n\0' pair, the
 * last line is still included in the returned array.
 *
 * This function assumes that the given string is `NULL` terminated, and that 
 * the length given also counts the NULL terminator (example: if `string` is 
 * "hello", then the length should be 6, not 5).
 *
 * If the input length excludes the NULL terminator, the final line may not be
 * included in the returned output.
 *
 * Each `Line` in the returned array contains two values: a length, and
 * the start index of the line relative to the original string.
 *
 * The start index of each `Line` is 0-based.
 *
 * The length of each line counts only non-NULL characters (i.e. if a line
 * contained the string "hello world\n", the length would be 11, not 12).
 *
 * Due to this, if you were to loop over the contents of a string using this
 * function, each line will contain either a newline, or, in the case of the
 * input string not ending in a '\n\0' pair, a NULL byte.
 *
 * The length of a line + its start index is also the start index of the line
 * after it in the string.
 *
 * Internally, the `Line` struct array is doubled in size each time it is
 * resized. When the function is done processing the given string, it will
 * resize the buffer to the correct size needed, to avoid wasting space on the
 * heap. This may fail if the function cannot allocate more memory for the new
 * smaller-sized buffer. In this case, the function will return the original
 * buffer, without any resizing.
 *
 * @param string The string to split. Is assumed to be `NULL` terminated.
 * @param length The length of the string, including the `NULL` terminator.
 * @param lineCount An output parameter that is set to the number of lines in
 *                  the given string.
 *
 * @return An array of `Line` structs, allocated on the heap.
 */
Line* splitByNewline(const char* string, const size_t length, size_t* lineCount) {
	Line* lines = calloc(1, sizeof(Line));

	if (lines == NULL) {
		return NULL;
	}

	size_t lnCount = 0;
	size_t lineBufSize = 1;

	size_t lineLength = 0;

	for (size_t i = 0; i < length; i++) {
		// since the string is assumed to be not only NULL terminated, but that
		// the length given also accounts for the NULL terminator, this check
		// here ensures that we catch the final line, even if the string doesn't
		// end with a '\n\0' pair.
		if (string[i] != '\n' && string[i] != '\0') {
			// incrementing here means we'll exclude counting any newlines or
			// NULL bytes
			lineLength++;

			continue;
		}

		lines[lnCount].length = lineLength;
		lines[lnCount].startIndex = i - lineLength;

		// reset lineLength
		lineLength = 0;

		lnCount++;

		if (lnCount < lineBufSize) {
			continue;
		}

		// double the buffer size
		lineBufSize *= 2;

		Line* tmp = realloc(lines, lineBufSize * sizeof(Line));

		if (tmp == NULL) {
			free(lines);

			return NULL;
		}

		lines = tmp;
	}

	if (lineCount != NULL) *lineCount = lnCount;

	// resize buffer down to needed size
	Line* tmp = calloc(lnCount, sizeof(Line));

	// couldn't allocate enough memory for the smaller buffer, so we'll return
	// the original
	//
	// probably means something *else* down the line will fail, but this is fine
	// (probably).
	if (tmp == NULL) {
		return lines;
	}

	// memcpy everything from the original
	memcpy(tmp, lines, lnCount * sizeof(Line));

	free(lines);

	return tmp;
}

ParserStatus tmpl_getContentsOfSection(const TMPLFile* tmplFile, const char* sectionName, size_t* length) {
	ParserStatus status;

	size_t lineCount;
	Line* lines = splitByNewline(tmplFile->contents, tmplFile->length, &lineCount);

	if (tmplFile == NULL || lines == NULL || sectionName == NULL) {
		status = (ParserStatus) {
			.errorMessage = "ERROR: input parameter was NULL\n",
			.hasValue = false
		};

		free(lines);

		return status;
	}

	size_t sectionLine = 0;
	bool foundSection = false;

	const char* fileContents = tmplFile->contents;

	for (size_t i = 0; i < lineCount; i++) {
		size_t startIndex = lines[i].startIndex;
		size_t lineLength = lines[i].length;

		if (lineLength < 3) {
			continue;
		}

		if (fileContents[startIndex] != '[' || fileContents[startIndex + lineLength - 1] != ']') {
			continue;
		}

		if (strlen(sectionName) == lineLength - 2 && strncmp(sectionName, fileContents + startIndex + 1, lineLength - 2) == 0) {
			foundSection = true;
			sectionLine = i;

			break;
		}
	}

	if (!foundSection) {
		status = (ParserStatus) {
			.errorMessage = "ERROR: failed to read from file: section not found",
			.hasValue = false
		};

		free(lines);

		return status;
	}

	// load contents
	char* sectionContents = malloc(READ_CHUNK);

	if (sectionContents == NULL) {
		status = (ParserStatus) {
			.errorMessage = "ERROR: not enough memory",
			.hasValue = false
		};

		free(lines);

		return status;
	}

	sectionContents[0] = '\0';

	size_t used = 0, bufSize = READ_CHUNK;

	if (sectionLine + 1 == lineCount) {
		status = (ParserStatus) {
			.sectionContents = sectionContents,
			.hasValue = true
		};

		free(lines);

		return status;
	}

	for (size_t i = sectionLine + 1; i < lineCount; i++) {
		size_t startIndex = lines[i].startIndex;
		size_t lineLength = lines[i].length;

		if (fileContents[startIndex] == '[' && fileContents[startIndex + lineLength - 1] == ']') {
			break;
		}

		for (size_t j = startIndex; j < startIndex + lineLength + 1; j++) {
			if (used + 1 <= bufSize) {
				sectionContents[used++] = fileContents[j];

				continue;
			}

			bufSize += READ_CHUNK + 1;

			char* tmp = realloc(sectionContents, bufSize);

			if (tmp == NULL) {
				status = (ParserStatus) {
					.errorMessage = "ERROR: not enough memory",
					.hasValue = false
				};

				free(sectionContents);
				free(lines);

				return status;
			}

			sectionContents = tmp;

			sectionContents[used++] = fileContents[j];
		}
	}

	free(lines);

	if (used > 0) {
		sectionContents[used - 1] = '\0';
		if (length) *length = used - 1;
	} else {
		sectionContents[0] = '\0';
		if (length) *length = 0;
	}

	status = (ParserStatus) {
		.sectionContents = sectionContents,
		.hasValue = true
	};

	return status;
}

void tmpl_free(TMPLFile* tmplFile) {
	if (tmplFile->contents) {
		free(tmplFile->contents);
	}

	tmplFile->contents = NULL;
	tmplFile->length = 0;

	free(tmplFile);
}
