#include <main.h>
#include <string.h>
#include <util/c_string.h>

String cstring_init(char* contents, size_t length) {
	String string = {
		.contents = contents,
		.length = length,
	};

	return string;
}

String cstring_initFromConst(const char* literal) {
	size_t len = strlen(literal);

	char* copy = malloc(len + 1);

	if (copy == NULL) {
		return cstring_init(NULL, 0);
	}

	memcpy(copy, literal, len);

	copy[len] = '\0';

	return cstring_init(copy, len);
}

void cstring_free(String* string) {
	if (string->contents) {
		free(string->contents);
	}

	string->contents = NULL;
	string->length = 0;
}
