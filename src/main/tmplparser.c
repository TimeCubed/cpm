#include <main.h>
#include <string.h>
#include <stdbool.h>
#include <tmplparser.h>

#define READ_CHUNK 128

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

// some implicit assumptions splitByNewline makes:
// - the length passed to it is the length of the string + 1 for a null byte
static Line* splitByNewline(const char* string, const size_t length, size_t* lineCount) {
	Line* lines = calloc(1, sizeof(Line));

	if (lines == NULL) {
		printf("tmplparser: ERROR: not enough memory\n");

		return NULL;
	}

	size_t lnCount = 0;
	size_t lineLength = 0;

	size_t i;

	// let me be completely clear, I have no freaking idea *how* this thing ends
	// up correctly saving the line data for the last line in the string despite
	// it sometimes not ending with a newline. it just does. I have no idea how,
	// or why, or what, it's just somehow saving that last line anyway and, you
	// know what, I'll just leave it be.
	for (i = 0; i < length; i++) {
		lineLength++;

		// genuinely, where on god's green earth is it actually finding a
		// newline. w h a t.
		//
		// ohh it was neovim
		// neovim was adding it because POSIX says to add it, ok
		if (string[i] == '\n') {
			// important note: the length of a line does not include the newline
			lines[lnCount].length = lineLength - 1;
			lines[lnCount].startIndex = i - lineLength + 1;

			lineLength = 0;

			lnCount++;

			Line* tmpLines = realloc(lines, (lnCount * 2) * sizeof(Line));

			if (tmpLines == NULL) {
				printf("tmplparser: ERROR: not enough memory\n");

				free(lines);

				return NULL;
			}

			lines = tmpLines;
		}
	}

	// if the input string isn't terminated by a newline character, this
	// function will miss the last line and not save it properly.
	// this accounts for that by saving that data anyway, since we always leave
	// enough space for 1 more line.
	//
	// not sure why this needs to also have 1 added to it, but go off windows.
	lines[lnCount].length = lineLength;
	// i here is actually 1 above what it was in the last line in the for loop,
	// meaning we have to subtract 1.
	// (i - 1) - lineLength + 1 = i - lineLength
	lines[lnCount].startIndex = i - lineLength;

	if (lineCount) *lineCount = ++lnCount;
	return lines;
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
			.errorMessage = "ERROR: failed to read from file: section not found\n",
			.hasValue = false
		};

		return status;
	}

	// load contents
	char* sectionContents = malloc(READ_CHUNK);

	if (sectionContents == NULL) {
		status = (ParserStatus) {
			.errorMessage = "ERROR: not enough memory\n",
			.hasValue = false
		};

		return status;
	}

	sectionContents[0] = '\0';

	size_t used = 0, bufSize = READ_CHUNK;

	if (sectionLine + 1 == lineCount) {
		status = (ParserStatus) {
			.sectionContents = sectionContents,
			.hasValue = true
		};

		return status;
	}

	for (size_t i = sectionLine + 1; i < lineCount; i++) {
		size_t startIndex = lines[i].startIndex;
		size_t lineLength = lines[i].length;

		if (fileContents[startIndex] == '[' && fileContents[startIndex + lineLength - 1] == ']') {
			break;
		}

		for (size_t j = startIndex; j < startIndex + lineLength + 1; j++) {
			if (used + 1 > bufSize) {
				bufSize += READ_CHUNK + 1;

				char* tmp = realloc(sectionContents, bufSize);

				if (tmp == NULL) {
					status = (ParserStatus) {
						.errorMessage = "ERROR: not enough memory\n",
						.hasValue = false
					};

					free(sectionContents);
					free(lines);

					return status;
				}

				sectionContents = tmp;
			}

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
