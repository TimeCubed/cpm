#include <main.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <tmplparser.h>

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
 * The length of each `Line` counts all characters within that line, including
 * any '\n' or '\0' characters. As an example, if a line contained the non-NULL-
 * terminated string "hello\n", the length of that line would be 6, not 5.
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
		lineLength++;
		// since the string is assumed to be not only NULL terminated, but that
		// the length given also accounts for the NULL terminator, this check
		// here ensures that we catch the final line, even if the string doesn't
		// end with a '\n\0' pair.
		if (string[i] != '\n' && string[i] != '\0') {
			// incrementing here means we'll exclude counting any newlines or
			// NULL bytes

			continue;
		}

		lines[lnCount].length = lineLength;
		lines[lnCount].startIndex = i - lineLength + 1;

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

#define isSectionHeader(string, si, lnlen) string[si] == '[' && string[si + lnlen - 2] == ']'
ParserStatus tmpl_getContentsOfSection(const TMPLFile* tmplFile, const char* sectionName, size_t* length) {
	size_t lineCount;
	Line* lines = splitByNewline(tmplFile->contents, tmplFile->length, &lineCount);

	if (tmplFile == NULL || lines == NULL || sectionName == NULL) {
		free(lines);

		return (ParserStatus) {
			.errorMessage = "ERROR: input parameter was NULL",
			.hasValue = false
		};
	}

	const char* fileContents = tmplFile->contents;

	bool foundSection = false;
	size_t sectionLine = 0;

	// find the line where the section is
	for (size_t i = 0; i < lineCount; i++) {
		size_t startIndex = lines[i].startIndex;
		size_t lineLength = lines[i].length;

		// the smallest possible section is something like
		// [a]\n
		// which is 4 characters long.
		//
		// if the current line is shorter than this, then it can't be a section
		// header.
		if (lineLength < 4) {
			continue;
		}

		if (!(isSectionHeader(fileContents, startIndex, lineLength))) {
			continue;
		}

		// if the line's length - 2 brackets - newline != sectionName's length
		if (strlen(sectionName) != lineLength - 3) {
			continue;
		}

		if (strncmp(sectionName, fileContents + startIndex + 1, lineLength - 3) == 0) {
			foundSection = true;
			sectionLine = i;

			break;
		}
	}

	// no section found
	if (!foundSection) {
		free(lines);

		return (ParserStatus) {
			.errorMessage = "ERROR: failed to read from file: section not found",
			.hasValue = false,
		};
	}

	char* sectionContents = malloc(READ_CHUNK);
	size_t used = 0, bufSize = READ_CHUNK;

	if (sectionContents == NULL) {
		free(lines);
		
		return (ParserStatus) {
			.errorMessage = "ERROR: not enough memory",
			.hasValue = false,
		};
	}

	// if the section header is the last line in the file
	if (sectionLine + 1 == lineCount) {
		sectionContents[0] = '\0';
		free(lines);

		return (ParserStatus) {
			.sectionContents = sectionContents,
			.hasValue = true,
		};
	}

	// if the next line after this section is another section => current section
	// has no contents
	if (isSectionHeader(fileContents, lines[sectionLine + 1].startIndex, lines[sectionLine + 1].length)) {
		sectionContents[0] = '\0';
		free(lines);

		return (ParserStatus) {
			.sectionContents = sectionContents,
			.hasValue = true,
		};
	}

	// copy contents
	for (size_t i = sectionLine + 1; i < lineCount; i++) {
		size_t startIndex = lines[i].startIndex;
		size_t lineLength = lines[i].length;

		if (isSectionHeader(fileContents, startIndex, lineLength)) {
			break;
		}

		for (size_t j = startIndex; j < startIndex + lineLength; j++) {
			sectionContents[used++] = fileContents[j];

			if (used + 1 > bufSize) {
				bufSize += READ_CHUNK + 1;

				char* tmp = realloc(sectionContents, bufSize);

				if (tmp == NULL) {
					free(lines);
					free(sectionContents);

					return (ParserStatus) {
						.errorMessage = "ERROR: not enough memory",
						.hasValue = false,
					};
				}

				sectionContents = tmp;
			}
		}
	}

	free(lines);

	sectionContents[used - 1] = '\0';

	if (length) *length = used - 1;

	return (ParserStatus) {
		.sectionContents = sectionContents, 
		.hasValue = true
	};
}

void tmpl_free(TMPLFile* tmplFile) {
	if (tmplFile->contents) {
		free(tmplFile->contents);
	}

	tmplFile->contents = NULL;
	tmplFile->length = 0;

	free(tmplFile);
}
