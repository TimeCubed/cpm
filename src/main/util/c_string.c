#include <main.h>
#include <c_string.h>

String cstring_init(char* contents, size_t length) {
	String string = {
		.contents = contents,
		.length = length,
	};

	return string;
}
