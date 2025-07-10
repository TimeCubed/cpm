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
ProjectConfig config_init(String name, Language language, Structure projectStructure) {
	ProjectConfig projectConfig = {
		.name = name,
		.language = language,
		.projectStructure = projectStructure,
	};

	return projectConfig;
}

void config_loadFiles(ProjectConfig* config) {
	TMPLFile* tmplFile;

	char* cwd = getCWD(getPathMax());

	char* homeDir = getHomeDir();
	char* configDir = getConfigDir();

	changeWD(homeDir);
	changeWD(configDir);

	switch (config->projectStructure) {
		case EXTENDED:
			tmplFile = tmpl_loadFile("templates-extended.tmpl");

			if (tmplFile) {
				break;
			}

			changeWD(cwd);
			free(cwd);

			tmplFile = tmpl_loadFile("resources/c/templates-extended.tmpl");

			if (tmplFile == NULL) {
				printf("ERROR: couldn't load template for extended structure\n");
				m_error = STATUS_FAIL;
				return;
			}

			break;
		case MINIMAL:
			tmplFile = tmpl_loadFile("templates-minimal.tmpl");

			if (tmplFile) {
				break;
			}

			changeWD(cwd);
			free(cwd);

			tmplFile = tmpl_loadFile("resources/c/templates-minimal.tmpl");

			if (tmplFile == NULL) {
				printf("ERROR: couldn't load template for minimal structure\n");
				m_error = STATUS_FAIL;
				return;
			}

			break;
		case NO_FOLDERS:
			tmplFile = tmpl_loadFile("templates-no-folders.tmpl");

			if (tmplFile) {
				break;
			}

			changeWD(cwd);
			free(cwd);

			tmplFile = tmpl_loadFile("resources/c/templates-no-folders.tmpl");

			if (tmplFile == NULL) {
				printf("ERROR: couldn't load template for no-folders structure\n");
				m_error = STATUS_FAIL;
				return;
			}

			break;
		default:
			m_error = STATUS_FAIL;
			return;
	}

	size_t mcLength, mhLength, mkLength;
	char* mainC    = tmpl_getContentsOfSection(tmplFile, "main.c", &mcLength);
	char* mainH    = tmpl_getContentsOfSection(tmplFile, "main.h", &mhLength);
	char* makefile = tmpl_getContentsOfSection(tmplFile, "makefile", &mkLength);

	if (mainC == NULL || mainH == NULL || makefile == NULL) {
		printf("ERROR: failed to read from template files\n");

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
