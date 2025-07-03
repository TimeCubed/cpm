#include <main.h>
#include <stdbool.h>
#include <string.h>

#define READ_CHUNK 128

typedef struct {
	char* contents;
	size_t length;
} TMPLFile;

typedef struct {
	char* contents;
	size_t length;
	size_t startIndex;
	size_t endIndex;
} Line;

TMPLFile* tmpl_loadFile(const char* path) {
	TMPLFile* tmplFile = calloc(1, sizeof(TMPLFile));

	FILE* file = fopen(path, "r");

	if (file == NULL) {
		perror("file open fail");
		return 0;
	}

	size_t used = 0, bufSize = READ_CHUNK;
	char* contents = malloc(READ_CHUNK);

	if (contents == NULL) {
		perror("malloc fail");
		return NULL;
	}

	while (1) {
		if (used + READ_CHUNK + 1 > bufSize) {
			bufSize += READ_CHUNK + 1;

			char* tmp = realloc(contents, bufSize);

			if (tmp == NULL) {
				free(contents);

				perror("realloc fail");
				return NULL;
			}

			contents = tmp;
		}

		int ret = fread(contents + used, sizeof(char), READ_CHUNK, file);

		if (ret == 0) {
			break;
		}

		used += ret;
	}

	char* tmp = realloc(contents, used + 1);

	if (tmp == NULL) {
		return NULL;
	}

	contents = tmp;
	contents[used] = '\0';

	tmplFile->length = used + 1;
	tmplFile->contents = contents;

	return tmplFile;
}

static Line* splitByNewline(const char* string, const size_t length, size_t* lineCount) {
	Line* lines = calloc(1, sizeof(Line));
	size_t lnCount = 0;

	const size_t chunk = 32;
	char* buf = malloc(chunk);
	size_t used = 0, bufSize = chunk;

	for (size_t i = 0; i < length; i++) {
		if (used + 2 > bufSize) {
			bufSize += chunk + 1;

			char* tmp = realloc(buf, bufSize);

			if (tmp == NULL) {
				free(buf);
				free(lines);

				return NULL;
			}

			buf = tmp;
		}

		buf[used++] = string[i];

		if (string[i] == '\n') {
			char* tmp = realloc(buf, used);

			if (tmp == NULL) {
				free(buf);
				free(lines);

				return NULL;
			}

			buf = tmp;
			buf[used - 1] = '\0';

			lines[lnCount].contents = buf;
			lines[lnCount].length = used - 1;
			lines[lnCount].startIndex = i - used + 1;
			lines[lnCount].endIndex = i;

			buf = malloc(chunk);
			
			if (buf == NULL) {
				return NULL;
			}

			used = 0, bufSize = chunk;

			lnCount++;

			Line* tmpLines = realloc(lines, (lnCount + 1) * sizeof(Line));

			if (tmpLines == NULL) {
				free(buf);
				free(lines);

				return NULL;
			}

			lines = tmpLines;
		}
	}

	*lineCount = lnCount;
	return lines;
}

char* tmpl_getContentsOfSection(TMPLFile* tmplFile, const char* sectionName, size_t* length) {
	size_t lineCount;
	Line* lines = splitByNewline(tmplFile->contents, tmplFile->length, &lineCount);

	if (lines == NULL || tmplFile == NULL || sectionName == NULL) {
		return NULL;
	}

	size_t i, startIndex;
	bool foundSection = false;

	for (i = 0; i < lineCount; i++) {
		char* line = lines[i].contents;
		size_t lineLength = lines[i].length;

		if (lineLength < 3) {
			continue;
		}

		if (line[0] != '[' || line[lineLength - 1] != ']') {
			continue;
		}

		if (strncmp(sectionName, line + 1, lineLength - 2) == 0) {
			foundSection = true;
			startIndex = lines[i].endIndex + 1;

			break;
		}
	}

	if (!foundSection) {
		return NULL;
	}

	// load contents
	char* buf = malloc(READ_CHUNK);
	size_t used = 0, bufSize = READ_CHUNK;

	char* contents = tmplFile->contents;
	for (size_t i = startIndex; i < tmplFile->length; i++) {
		if (contents[i] == '[') {
			break;
		}

		if (used + READ_CHUNK + 1 > bufSize) {
			bufSize += READ_CHUNK + 1;

			char* tmp = realloc(buf, bufSize);

			if (tmp == NULL) {
				return NULL;
			}

			buf = tmp;
		}

		buf[used++] = contents[i];
	}

	buf[used - 1] = '\0';
	if (length) *length = used - 1;

	return buf;
}
