#ifndef PATHS_H
#define PATHS_H

#include <windows.h>
#include <iostream>

class Paths
{
protected:
    static std::string expandVariable(char *variable);

public:
    __declspec(dllexport) static std::string getLogPath();
    __declspec(dllexport) static std::string getDatabasePath();
    __declspec(dllexport) static std::string getConfigPath();
};

#endif // PATHS_H
