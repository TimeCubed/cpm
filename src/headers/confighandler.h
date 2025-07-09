#ifndef CONFIG_HANDLER
#define CONFIG_HANDLER

#include <main.h>
#include <c_string.h>

typedef enum {
	C,
	CPP,
} Language;

typedef enum {
	EXTENDED,
	MINIMAL,
	NO_FOLDERS,
} Structure;

typedef struct {
	String name;
	Language language;
	Structure projectStructure;

	// hardcoding the files for the project is likely a bad idea if I want to
	// support other languages in the future, but it's also the easiest method 
	// currently.
	//
	// I could probably add a struct representing a file (file name, path, etc.)
	// and then have an array of them in here, instead of hardcoding each one
	// like this, but this would require extra changes.
	//
	// probably, I'll need to redo the way I handle configs so I could specify
	// not only where each file goes, but also the project structure too. this
	// would be a somewhat large undertaking as well, so I'll opt to not do that
	// for now. probably save it for a v3 I guess?

	String mainC;
	String mainH;
	String makefile;
} ProjectConfig;

/**
 * Initializes a `ProjectConfig` object on the stack with the given parameters.
 *
 * @param name The project name.
 * @param language The project's language.
 * @param projectStructure The project's structure.
 *
 * @return A `ProjectConfig` object allocated on the stack.
 */
ProjectConfig config_init(String name, Language language, Structure projectStructure);

/**
 * Loads all files necessary for the current configuration.
 *
 * @param config A pointer to the project's configuration.
 */
void config_loadFiles(ProjectConfig* config);

/**
 * Frees all strings stored inside `config`.
 *
 * @param config The configuration to free.
 */
void config_free(ProjectConfig config);

/**
 * Returns the error code of the last run config handler function.
 *
 * @return `STATUS_OK` if ok, `STATUS_FAIL` otherwise.
 */
int config_checkError(void);

#endif
