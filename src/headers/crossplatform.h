#ifndef CROSS_PLATFORM
#define CROSS_PLATFORM

#include <main.h>

#ifdef LINUX
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/limits.h>
#endif

#ifdef WINDOWS
#include <windows.h>
#include <errhandlingapi.h>
#endif

char* getExecutablePath(size_t* len);
size_t getPathMax(void);
char* getCWD(size_t maxLength);
int changeWD(char* path);
int makeDirectory(char* path, int mode);
char* getHomeDir(void);
char* getConfigDir(void);
char* translatePath(const char* path);

#endif
