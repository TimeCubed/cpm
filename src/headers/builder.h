#ifndef BUILDER
#define BUILDER

#include <confighandler.h>

/**
 * Builds the project using the given configuration.
 *
 * @param config The project's configuration.
 *
 * @return `STATUS_OK` if successful, `STATUS_FAIL` otherwise.
 */
int buildProject(ProjectConfig config);

#endif
