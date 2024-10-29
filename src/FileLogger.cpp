#include <iostream>
#include "FileLogger.h"

FileLogger::FileLogger(std::string fileFullPath, bool enableConsoleLog) 
    : 
    LogFile(fileFullPath, std::ofstream::out | std::ofstream::trunc),
    EnableConsoleLog(enableConsoleLog)
{
}

FileLogger::~FileLogger()
{
    LogFile.close();
}

void FileLogger::LogInfo(const std::string& logText)
{
    if (LogFile.is_open())
    {
        LogFile << "[INFO    ] " <<  logText << std::endl;
    }

    if (EnableConsoleLog)
    {
        std::cout << "[INFO    ] " << logText << std::endl;
    }
}

void FileLogger::LogWarning(const std::string& logText)
{
    if (LogFile.is_open())
    {
        LogFile << "[WARNING ] " << logText << std::endl;
    }

    if (EnableConsoleLog)
    {
        std::cout << "[WARNING ] " << logText << std::endl;
    }
}

void FileLogger::LogError(const std::string& logText)
{
    if (LogFile.is_open())
    {
        LogFile << "[ERROR   ] " << logText << std::endl;
    }

    if (EnableConsoleLog)
    {
        std::cerr << "[ERROR   ] " << logText << std::endl;
    }
}

