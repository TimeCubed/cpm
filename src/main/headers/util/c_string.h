#ifndef C_STRING
#define C_STRING

#include <main.h>

typedef struct {
	char* contents;
	size_t length;
} String;

String cstring_init(char* contents, size_t length);

String cstring_initFromConst(const char* literal);

void cstring_free(String* string);

#endif
