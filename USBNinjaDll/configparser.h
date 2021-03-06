#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <algorithm>

#include "paths.h"

struct sqlSettings
{
    std::string ip,
    username, password,
    database;
    int port;
};

class ConfigParser
{
    std::string configFile;
    std::string configFileData;
    std::vector<std::string> var, value;

    __declspec(dllexport) void parseLine(std::string line);
    __declspec(dllexport) int findPos(std::string varStr);

public:
    __declspec(dllexport) ConfigParser(char *cfgFile) : configFile(cfgFile)
    {
        //remove(configFile.c_str());
        std::string dataTemp;
        std::fstream hFile(configFile.c_str(), std::fstream::in);
        while (hFile.good())
        {
            std::getline(hFile, dataTemp);
            if (dataTemp.length() > 0)
            {
                if ((dataTemp.at(0) != '#') && ((dataTemp.find('=')) != std::string::npos))
                {
                    configFileData.append(dataTemp);
                    parseLine(dataTemp);
                }
            }
        }
        hFile.close();
    }

    __declspec(dllexport) std::string getValue(std::string varStr);
    __declspec(dllexport) void setValue(std::string varStr, std::string valueStr);
    __declspec(dllexport) std::string getData();
    __declspec(dllexport) sqlSettings getSqlSettings();
};

#endif // CONFIGPARSER_H
