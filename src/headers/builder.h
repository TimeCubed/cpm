#ifndef BUILDER
#define BUILDER

#include <confighandler.h>

/**
 * Builds the project using the current configuration.
 *
 * @return `STATUS_OK` if successful, `STATUS_FAIL` otherwise.
 */
int buildProject(void);

#endif
