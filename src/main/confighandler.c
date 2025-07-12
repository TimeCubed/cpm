#include <main.h>
#include <stdbool.h>
#include <c_string.h>
#include <cliswitch.h>
#include <tmplparser.h>
#include <crossplatform.h>
#include <confighandler.h>

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

int config_loadFiles(ProjectConfig* config) {
	TMPLFile* tmplFile = NULL;

	char* cwd = getCWD(getPathMax());

	char* homeDir = getHomeDir();
	char* configDir = getConfigDir();

	changeWD(homeDir);
	changeWD(configDir);

	char* userTemplatePath = NULL;
	char* defaultTemplatePath = NULL;

	switch (config->projectStructure) {
		case EXTENDED:
			if (config->language == C) {
				userTemplatePath = "c/templates-extended.tmpl";
				defaultTemplatePath = "resources/c/templates-extended.tmpl";
			} else if (config->language == CPP) {
				userTemplatePath = "cpp/templates-extended.tmpl";
				defaultTemplatePath = "resources/cpp/templates-extended.tmpl";
			}
			break;
		case MINIMAL:
			if (config->language == C) {
				userTemplatePath = "c/templates-minimal.tmpl";
				defaultTemplatePath = "resources/c/templates-minimal.tmpl";
			} else if (config->language == CPP) {
				userTemplatePath = "cpp/templates-minimal.tmpl";
				defaultTemplatePath = "resources/cpp/templates-minimal.tmpl";
			}
			break;
		case NO_FOLDERS:
			if (config->language == C) {
				userTemplatePath = "c/templates-no-folders.tmpl";
				defaultTemplatePath = "resources/c/templates-no-folders.tmpl";
			} else if (config->language == CPP) {
				userTemplatePath = "cpp/templates-no-folders.tmpl";
				defaultTemplatePath = "resources/cpp/templates-no-folders.tmpl";
			}
			break;
		default:
			printf("cpm: ERROR: unknown error occurred\n");
			return STATUS_FAIL;
	}

	if (userTemplatePath == NULL || defaultTemplatePath == NULL) {
		printf("cpm: ERROR: unknown error occurred\n");
		return STATUS_FAIL;
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

			return STATUS_FAIL;
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

		return STATUS_FAIL;
	}

	config->mainC    = cstring_init(mainC, mcLength);
	config->mainH    = cstring_init(mainH, mhLength);
	config->makefile = cstring_init(makefile, mkLength);

	free(tmplFile);

	return STATUS_OK;
}

void config_free(ProjectConfig config) {
	if (config.mainC.contents != NULL) free(config.mainC.contents);
	if (config.mainH.contents != NULL) free(config.mainH.contents);
	if (config.makefile.contents != NULL) free(config.makefile.contents);
}
