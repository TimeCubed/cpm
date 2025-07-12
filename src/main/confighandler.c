#include <main.h>
#include <stdbool.h>
#include <c_string.h>
#include <cliswitch.h>
#include <tmplparser.h>
#include <crossplatform.h>
#include <confighandler.h>

static ProjectConfig* currentConfig;
static int m_error = STATUS_OK;

ProjectConfig config_init(void) {
	ProjectConfig projectConfig = {
		.name = cstring_init("default", 8),
		.language = C,
		.projectStructure = EXTENDED,
		.defaultTemplates = false,
		.mainC = cstring_init("", 1),
		.mainH = cstring_init("", 1),
		.makefile = cstring_init("", 1),
	};

	return projectConfig;
}

void config_loadFiles(void) {
	if (!config_isCurrent()) {
		printf("ERROR: no config found\n");

		m_error = STATUS_FAIL;
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
			printf("ERROR: unknown error occurred\n");

			m_error = STATUS_FAIL;
			return;
	}

	if (userTemplatePath == NULL || defaultTemplatePath == NULL) {
		printf("ERROR: unknown error occurred\n");
		m_error = STATUS_FAIL;
		return;
	}

	if (!currentConfig->defaultTemplates) {
		tmplFile = tmpl_loadFile(userTemplatePath);
	}

	if (tmplFile == NULL || currentConfig->defaultTemplates) {
		changeWD(cwd);

		tmplFile = tmpl_loadFile(defaultTemplatePath);

		if (tmplFile == NULL) {
			printf("cpm: ERROR: could not load any template files for the current structure (no default or user templates found)\n");

			free(cwd);

			m_error = STATUS_FAIL;
			return;
		}
	} else {
		printf("cpm: found user config\n");
	}

	free(cwd);

	size_t mcLength, mhLength, mkLength;
	char* mainC    = tmpl_getContentsOfSection(tmplFile, "main.c", &mcLength);
	char* mainH    = tmpl_getContentsOfSection(tmplFile, "main.h", &mhLength);
	char* makefile = tmpl_getContentsOfSection(tmplFile, "makefile", &mkLength);

	if (mainC == NULL || mainH == NULL || makefile == NULL) {
		free(tmplFile);

		m_error = STATUS_FAIL;
		return;
	}

	currentConfig->mainC    = cstring_init(mainC, mcLength);
	currentConfig->mainH    = cstring_init(mainH, mhLength);
	currentConfig->makefile = cstring_init(makefile, mkLength);

	free(tmplFile);

	m_error = STATUS_OK;
	return;
}

void config_setName(String name) {
	if (!config_isCurrent()) {
		printf("ERROR: no config found\n");

		m_error = STATUS_FAIL;
		return;
	}

	m_error = STATUS_OK;

	if (currentConfig->name.contents) free(currentConfig->name.contents);

	currentConfig->name = name;
}

void config_setLanguage(Language language) {
	if (!config_isCurrent()) {
		printf("ERROR: no config found\n");

		m_error = STATUS_FAIL;
		return;
	}

	m_error = STATUS_OK;
	currentConfig->language = language;
}

void config_setStructure(Structure projectStructure) {
	if (!config_isCurrent()) {
		printf("ERROR: no config found\n");

		m_error = STATUS_FAIL;
		return;
	}

	m_error = STATUS_OK;
	currentConfig->projectStructure = projectStructure;
}

void config_setDefaultTemplates(bool defaultTemplates) {
	if (!config_isCurrent()) {
		printf("ERROR: no config found\n");

		m_error = STATUS_FAIL;
		return;
	}

	m_error = STATUS_OK;
	currentConfig->defaultTemplates = defaultTemplates;
}

void config_freeCurrent(void) {
	if (!config_isCurrent()) {
		printf("ERROR: no config found\n");

		m_error = STATUS_FAIL;
		return;
	}

	if (currentConfig->name.contents) free(currentConfig->name.contents);
	if (currentConfig->mainC.contents) free(currentConfig->mainC.contents);
	if (currentConfig->mainH.contents) free(currentConfig->mainH.contents);
	if (currentConfig->makefile.contents) free(currentConfig->makefile.contents);

	currentConfig = NULL;
	m_error = STATUS_OK;
}

void config_makeCurrent(ProjectConfig* config) {
	if (config == NULL) {
		printf("ERROR: config was NULL\n");

		m_error = STATUS_FAIL;
		return;
	}

	currentConfig = config;
	m_error = STATUS_OK;
}

bool config_isCurrent(void) {
	return currentConfig != NULL;
}

ProjectConfig config_getCurrent(void) {
	if (!config_isCurrent()) {
		printf("ERROR: no config found\n");

		m_error = STATUS_FAIL;
		return config_init();
	}

	m_error = STATUS_OK;
	return *currentConfig;
}

int config_checkError(void) {
	return m_error;
}
