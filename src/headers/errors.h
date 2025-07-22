#ifndef ERRORS
#define ERRORS

typedef enum {
	// generic errors
	STATUS_OK,
	STATUS_FAIL,

	// uncategorized
	ERROR_NOT_ENOUGH_MEMORY,

	// confighandler errors
	ERROR_NO_CURRENT_CONFIG,
	ERROR_NO_TEMPLATE_FILES,
	ERROR_KEY_SECTION_NOT_FOUND,

	// file I/O errors
	ERROR_FILE_OPEN_FAIL,
	ERROR_FILE_WRITE_FAIL,
	ERROR_CREATE_DIRECTORY_FAIL,

	// CLI parsing related errors
	ERROR_INVALID_ARGUMENT,
	ERROR_MULTIPLE_NON_SWITCH_ARGUMENTS,
} error_t;

#define isError(expr) expr != STATUS_OK

#endif
