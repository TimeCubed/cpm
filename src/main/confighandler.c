#include "errors.h"
#include <main.h>
#include <stdbool.h>
#include <c_string.h>
#include <cliswitch.h>
#include <tmplparser.h>
#include <crossplatform.h>
#include <confighandler.h>

int m_error = 0;

// evil object oriented programming constructor? IN MY C??
// IMPOSSIBLE
ProjectConfig config_init(String name, Language language, Structure projectStructure, bool defaultTemplates) {
	ProjectConfig projectConfig = {
		.name = name,
		.language = language,
		.projectStructure = projectStructure,
		.defaultTemplates = defaultTemplates,
	};

	return projectConfig;
}

void config_loadFiles(ProjectConfig* config) {
	TMPLFile* tmplFile = NULL;

	char* cwd = getCWD(getPathMax());

	char* homeDir = getHomeDir();
	char* configDir = getConfigDir();

	changeWD(homeDir);
	changeWD(configDir);

	char* userTemplatePath;
	char* defaultTemplatePath;

	switch (config->projectStructure) {
		case EXTENDED:
			userTemplatePath = "c/templates-extended.tmpl";
			defaultTemplatePath = "resources/c/templates-extended.tmpl";
			break;
		case MINIMAL:
			userTemplatePath = "c/templates-minimal.tmpl";
			defaultTemplatePath = "resources/c/templates-minimal.tmpl";
			break;
		case NO_FOLDERS:
			userTemplatePath = "c/templates-no-folders.tmpl";
			defaultTemplatePath = "resources/c/templates-no-folders.tmpl";
			break;
	}

	if (!config->defaultTemplates) {
		tmplFile = tmpl_loadFile(userTemplatePath);
	}

	if (tmplFile == NULL || config->defaultTemplates) {
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

	config->mainC    = cstring_init(mainC, mcLength);
	config->mainH    = cstring_init(mainH, mhLength);
	config->makefile = cstring_init(makefile, mkLength);

	free(tmplFile);

	m_error = STATUS_OK;
	return;
}

void config_free(ProjectConfig config) {
	if (config.mainC.contents != NULL) free(config.mainC.contents);
	if (config.mainH.contents != NULL) free(config.mainH.contents);
	if (config.makefile.contents != NULL) free(config.makefile.contents);

	m_error = STATUS_OK;
}

int config_checkError(void) {
	return m_error;
}
