#ifndef CLI_SWITCH
#define CLI_SWITCH

typedef void (*Func)(void);

/**
 * Adds a switch and its corresponding callback function to a list of switches
 * and callbacks.
 *
 * @param switchName The name of the switch. Max length of 32 characters.
 * @param callback The function that's called if the switch is found in argv.
 *
 * @return `STATUS_OK` if successful, `STATUS_FAIL` otherwise
 */
int addSwitch(const char* switchName, Func callback);

/**
 * Parses argv and looks for any string that matches a string in the list of
 * switches added and calls that switch's callback function.
 */
void parseArgv(int argc, char** argv);

#endif
