#include <main.h>
#include <tmplparser.h>
#include <stdbool.h>
#include <string.h>

#define READ_CHUNK 128

TMPLFile* tmpl_loadFile(const char* path) {
	TMPLFile* tmplFile = calloc(1, sizeof(TMPLFile));

	if (tmplFile == NULL) {
		return NULL;
	}

	FILE* file = fopen(path, "r");

	if (file == NULL) {
		free(tmplFile);

		return NULL;
	}

	size_t used = 0, bufSize = READ_CHUNK;
	char* contents = malloc(READ_CHUNK);

	if (contents == NULL) {
		free(tmplFile);
		fclose(file);

		return NULL;
	}

	while (1) {
		if (used + READ_CHUNK + 1 > bufSize) {
			bufSize += READ_CHUNK + 1;

			char* tmp = realloc(contents, bufSize);

			if (tmp == NULL) {
				printf("ERROR: not enough memory\n");

				free(contents);
				free(tmplFile);
				fclose(file);

				return NULL;
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

	return tmplFile;
}

// some implicit assumptions splitByNewline makes:
// - the length passed to it is the length of the string + 1 for a null byte
static Line* splitByNewline(const char* string, const size_t length, size_t* lineCount) {
	Line* lines = calloc(1, sizeof(Line));

	if (lines == NULL) {
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
	// importantly, this also ends up leaving an empty line if the string *was*
	// terminated by a newline + null byte, which may lead to problems down the
	// line if this implementation changes, as some code might depend on that 
	// extra line existing in the buffer.
	lines[lnCount].length = lineLength - 1;
	lines[lnCount].startIndex = i - lineLength + 1;

	if (lineCount) *lineCount = lnCount;
	return lines;
}

char* tmpl_getContentsOfSection(const TMPLFile* tmplFile, const char* sectionName, size_t* length) {
	size_t lineCount;
	Line* lines = splitByNewline(tmplFile->contents, tmplFile->length, &lineCount);

	if (tmplFile == NULL || lines == NULL || sectionName == NULL) {
		return NULL;
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
		printf("ERROR: failed to read from template file: section not found: %s\n", sectionName);
		return NULL;
	}

	// load contents
	char* sectionContents = malloc(READ_CHUNK);

	if (sectionContents == NULL) {
		return NULL;
	}

	size_t used = 0, bufSize = READ_CHUNK;

	if (sectionLine + 1 == lineCount) {
		return "";
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
					printf("ERROR: not enough memory\n");

					free(sectionContents);
					free(lines);

					return NULL;
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

	return sectionContents;
}
