#ifndef CLI_SWITCH
#define CLI_SWITCH

typedef void (*Func)(void);

int addSwitch(const char* switchName, Func callback);
void parseArgv(int argc, char** argv);

#endif
