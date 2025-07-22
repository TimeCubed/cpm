#include "errors.h"
#include <main.h>
#include <stdbool.h>
#include <tmplparser.h>
#include <confighandler.h>
#include <util/c_string.h>
#include <util/cliswitch.h>
#include <util/crossplatform.h>

#define verbose(...) if (currentConfig != NULL) if (currentConfig->verbose) printf(__VA_ARGS__)

static ProjectConfig* currentConfig;
static error_t m_error = STATUS_OK;

ProjectConfig config_init(void) {
	ProjectConfig projectConfig = {
		.name = cstring_initFromConst("default"),
		.language = C,
		.projectStructure = EXTENDED,
		.defaultTemplates = false,
		.verbose = false,
		.mainC = cstring_initFromConst(""),
		.mainH = cstring_initFromConst(""),
		.makefile = cstring_initFromConst(""),
	};

	return projectConfig;
}

void config_loadFiles(void) {
	if (!config_isCurrent()) {
		printf("confighandler: ERROR: no config found\n");

		m_error = ERROR_NO_CURRENT_CONFIG;
		return;
	}

	TMPLFile* tmplFile = NULL;

	char* cwd = getCWD(getPathMax());

	char* homeDir = getHomeDir();
	char* configDir = getConfigDir();

	changeWD(homeDir);
	changeWD(configDir);

	char* userTemplatePath = NULL;
	char* defaultTemplatePath = NULL;

	switch (currentConfig->projectStructure) {
		case EXTENDED:
			if (currentConfig->language == C) {
				userTemplatePath = "c/templates-extended.tmpl";
				defaultTemplatePath = "resources/c/templates-extended.tmpl";
			} else if (currentConfig->language == CPP) {
				userTemplatePath = "cpp/templates-extended.tmpl";
				defaultTemplatePath = "resources/cpp/templates-extended.tmpl";
			}
			break;
		case MINIMAL:
			if (currentConfig->language == C) {
				userTemplatePath = "c/templates-minimal.tmpl";
				defaultTemplatePath = "resources/c/templates-minimal.tmpl";
			} else if (currentConfig->language == CPP) {
				userTemplatePath = "cpp/templates-minimal.tmpl";
				defaultTemplatePath = "resources/cpp/templates-minimal.tmpl";
			}
			break;
		case NO_FOLDERS:
			if (currentConfig->language == C) {
				userTemplatePath = "c/templates-no-folders.tmpl";
				defaultTemplatePath = "resources/c/templates-no-folders.tmpl";
			} else if (currentConfig->language == CPP) {
				userTemplatePath = "cpp/templates-no-folders.tmpl";
				defaultTemplatePath = "resources/cpp/templates-no-folders.tmpl";
			}
			break;
		default:
			printf("confighandler: ERROR: unknown error occurred\n");

			m_error = STATUS_FAIL;
			return;
	}

	if (userTemplatePath == NULL || defaultTemplatePath == NULL) {
		printf("confighandler: ERROR: unknown error occurred\n");
		m_error = STATUS_FAIL;
		return;
	}

	userTemplatePath = translatePath(userTemplatePath);
	defaultTemplatePath = translatePath(defaultTemplatePath);

	if (!currentConfig->defaultTemplates) {
		LoaderStatus status = tmpl_loadFile(userTemplatePath);

		if (status.hasValue == true) {
			tmplFile = status.tmplFile;
		} else {
			verbose("%s", status.errorMessage);
		}
	}

	if (tmplFile == NULL || currentConfig->defaultTemplates) {
		changeWD(cwd);

		LoaderStatus status = tmpl_loadFile(defaultTemplatePath);

		if (status.hasValue == false) {
			verbose("%s", status.errorMessage);

			printf("confighandler: ERROR: could not load any template files for the current structure (no default or user templates found)\n");

			free(cwd);

			m_error = ERROR_NO_TEMPLATE_FILES;
			return;
		}

		tmplFile = status.tmplFile;
	} else {
		verbose("confighandler: found user config\n");
	}

	free(cwd);

	size_t mcLength, mhLength, mkLength;
	char* mainC    = NULL;
	char* mainH    = NULL;
	char* makefile = NULL;

	ParserStatus status = tmpl_getContentsOfSection(tmplFile, "main.c", &mcLength);

	if (status.hasValue) {
		mainC = status.sectionContents;
	} else {
		printf("%s: 'main.c'", status.errorMessage);

		tmpl_free(tmplFile);

		m_error = ERROR_KEY_SECTION_NOT_FOUND;
		return;
	}

	status = tmpl_getContentsOfSection(tmplFile, "main.h", &mhLength);

	if (status.hasValue) {
		mainH = status.sectionContents;
	} else {
		printf("%s: 'main.h'", status.errorMessage);

		tmpl_free(tmplFile);

		m_error = ERROR_KEY_SECTION_NOT_FOUND;
		return;
	}

	status = tmpl_getContentsOfSection(tmplFile, "makefile", &mkLength);

	if (status.hasValue) {
		makefile = status.sectionContents;
	} else {
		printf("%s: 'makefile'", status.errorMessage);

		tmpl_free(tmplFile);

		m_error = ERROR_KEY_SECTION_NOT_FOUND;
		return;
	}

	currentConfig->mainC    = cstring_init(mainC, mcLength);
	currentConfig->mainH    = cstring_init(mainH, mhLength);
	currentConfig->makefile = cstring_init(makefile, mkLength);

	tmpl_free(tmplFile);

	m_error = STATUS_OK;
	return;
}

void config_setName(String name) {
	if (!config_isCurrent()) {
		printf("confighandler: ERROR: no config found\n");

		m_error = ERROR_NO_CURRENT_CONFIG;
		return;
	}

	m_error = STATUS_OK;

	if (currentConfig->name.contents != NULL) free(currentConfig->name.contents);

	currentConfig->name = name;
}

void config_setLanguage(Language language) {
	if (!config_isCurrent()) {
		printf("confighandler: ERROR: no config found\n");

		m_error = ERROR_NO_CURRENT_CONFIG;
		return;
	}

	m_error = STATUS_OK;
	currentConfig->language = language;
}

void config_setStructure(Structure projectStructure) {
	if (!config_isCurrent()) {
		printf("confighandler: ERROR: no config found\n");

		m_error = ERROR_NO_CURRENT_CONFIG;
		return;
	}

	m_error = STATUS_OK;
	currentConfig->projectStructure = projectStructure;
}

void config_setDefaultTemplates(bool defaultTemplates) {
	if (!config_isCurrent()) {
		printf("confighandler: ERROR: no config found\n");

		m_error = ERROR_NO_CURRENT_CONFIG;
		return;
	}

	m_error = STATUS_OK;
	currentConfig->defaultTemplates = defaultTemplates;
}

void config_freeCurrent(void) {
	if (!config_isCurrent()) {
		printf("confighandler: ERROR: no config found\n");

		m_error = ERROR_NO_CURRENT_CONFIG;
		return;
	}

	if (currentConfig->name.contents) free(currentConfig->name.contents);
	if (currentConfig->mainC.contents) free(currentConfig->mainC.contents);
	if (currentConfig->mainH.contents) free(currentConfig->mainH.contents);
	if (currentConfig->makefile.contents) free(currentConfig->makefile.contents);

	currentConfig = NULL;
	m_error = STATUS_OK;
}

void config_setVerbose(bool verbose) {
	if (!config_isCurrent()) {
		printf("confighandler: ERROR: no config found\n");

		m_error = ERROR_NO_CURRENT_CONFIG;
		return;
	}

	currentConfig->verbose = verbose;
}

void config_makeCurrent(ProjectConfig* config) {
	if (config == NULL) {
		printf("confighandler: ERROR: no config found\n");

		m_error = ERROR_NO_CURRENT_CONFIG;
		return;
	}

	currentConfig = config;
	m_error = STATUS_OK;
}

ProjectConfig config_getCurrent(void) {
	if (!config_isCurrent()) {
		printf("confighandler: ERROR: no config found\n");

		m_error = ERROR_NO_CURRENT_CONFIG;
		return config_init();
	}

	m_error = STATUS_OK;
	return *currentConfig;
}

bool config_isCurrent(void) {
	return currentConfig != NULL;
}

int config_checkError(void) {
	return m_error;
}
