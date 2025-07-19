#ifndef CONFIG_HANDLER
#define CONFIG_HANDLER

#include <main.h>
#include <stdbool.h>
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

	bool defaultTemplates;
	bool verbose;

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
 * Returns a stack-allocated `ProjectConfig` object with default values.
 * Does not set the returned config as the current config.
 *
 * m_error is not set when using this function.
 *
 * @return A `ProjectConfig` object allocated on the stack.
 */
ProjectConfig config_init(void);

/**
 * Loads all files necessary for the current configuration. Uses the current
 * config, set by `config_init` or `config_makeCurrent`.
 *
 * m_error is set to `STATUS_FAIL` if there was a problem loading the correct
 * files, reading from said files, allocating memory, or if there is no current
 * config. Otherwise, it is set to `STATUS_OK`.
 *
 * @param config A pointer to the project's configuration.
 */
void config_loadFiles(void);

/**
 * Frees the old name (if not `NULL`) and sets the name of the current config to
 * `name`.
 *
 * If there is no current config, m_error is set to `STATUS_FAIL` and this
 * function won't do anything. Otherwise, m_error is set to `STATUS_OK`.
 *
 * @param name The new name for the config.
 */
void config_setName(String name);

/**
 * Sets the language of the current config to `language`.
 *
 * If there is no current config, m_error is set to `STATUS_FAIL` and this
 * function won't do anything. Otherwise, m_error is set to `STATUS_OK`.
 *
 * @param language The new language for the config.
 */
void config_setLanguage(Language language);

/**
 * Sets the structure of the current config to `projectStructure`.
 *
 * If there is no current config, m_error is set to `STATUS_FAIL` and this
 * function won't do anything. Otherwise, m_error is set to `STATUS_OK`.
 *
 * @param projectStructure The new structure for the config.
 */
void config_setStructure(Structure projectStructure);

/**
 * Sets whether to force use the default templates.
 *
 * If there is no current config, m_error is set to `STATUS_FAIL` and this
 * function won't do anything. Otherwise, m_error is set to `STATUS_OK`.
 *
 * @param defaultTemplates `true` to force using default templates,
 *                         `false` otherwise.
 */
void config_setDefaultTemplates(bool defaultTemplates);

/**
 * Sets verbose logging output.
 *
 * If there is no current config, m_error is set to `STATUS_FAIL` and this
 * function won't do anything. Otherwise, m_error is set to `STATUS_OK`.
 *
 * @param verbose `true` to allow verbose logging,
 *                `false` to disallow verbose logging.
 */
void config_setVerbose(bool verbose);

/**
 * Frees all strings stored inside the current config.
 *
 * If there is no current config, m_error is set to `STATUS_FAIL` and this
 * function won't do anything. Otherwise, m_error is set to `STATUS_OK`.
 *
 * @param config The configuration to free.
 */
void config_freeCurrent(void);

/**
 * Makes the given project configuration the current configuration to be used in
 * other functions.
 *
 * m_error is set to `STATUS_FAIL` if `config` is `NULL`.
 *
 * @param config The pointer to a config to set as the current config.
 */
void config_makeCurrent(ProjectConfig* config);

/**
 * Returns the current config (read-only).
 *
 * If there is no current config, m_error is set to `STATUS_FAIL`, and returns a
 * default config from `config_init`. Otherwise, m_error is set to `STATUS_OK`.
 */
ProjectConfig config_getCurrent(void);

/**
 * Checks if there is a current config bound.
 *
 * m_error is not set when using this function.
 *
 * @return `true` if there is a current config, `false` otherwise.
 */
bool config_isCurrent(void);

/**
 * Returns the status of the last run config handler function.
 * `STATUS_OK` if successful, `STATUS_FAIL` otherwise.
 */
int config_checkError(void);

#endif
