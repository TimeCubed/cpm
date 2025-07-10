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
				free(contents);
				free(tmplFile);
				fclose(file);

				perror("realloc fail");
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

static Line* splitByNewline(const char* string, const size_t length, size_t* lineCount) {
	Line* lines = calloc(1, sizeof(Line));

	if (lines == NULL) {
		return NULL;
	}

	size_t lnCount = 0;
	size_t lineLength = 0;

	for (size_t i = 0; i < length; i++) {
		lineLength++;

		if (string[i] == '\n') {
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

	*lineCount = lnCount;
	return lines;
}

char* tmpl_getContentsOfSection(const TMPLFile* tmplFile, const char* sectionName, size_t* length) {
	size_t lineCount;
	Line* lines = splitByNewline(tmplFile->contents, tmplFile->length, &lineCount);

	if (tmplFile == NULL || lines == NULL || sectionName == NULL) {
		return NULL;
	}

	size_t sectionIndex;
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
			sectionIndex = startIndex + lineLength + 1;

			break;
		}
	}

	free(lines);

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

	for (size_t i = sectionIndex; i < tmplFile->length; i++) {
		if (fileContents[i] == '[') {
			break;
		}

		if (used + READ_CHUNK + 1 > bufSize) {
			bufSize += READ_CHUNK + 1;

			char* tmp = realloc(sectionContents, bufSize);

			if (tmp == NULL) {
				free(sectionContents);

				return NULL;
			}

			sectionContents = tmp;
		}

		sectionContents[used++] = fileContents[i];
	}

	if (used > 0) {
		sectionContents[used - 1] = '\0';
		if (length) *length = used - 1;
	} else {
		sectionContents[0] = '\0';
		if (length) *length = 0;
	}

	return sectionContents;
}
