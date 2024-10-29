#pragma once

#include <fstream>
#include "ILogger.h"

class FileLogger : public ILogger
{
public:
    FileLogger(std::string fileFullPath, bool enableConsoleLog);
    virtual ~FileLogger();

    void LogInfo(std::string logText)        override;
    void LogWarning(std::string logText)     override;
    void LogError(std::string logText)       override;

private:
    std::ofstream LogFile;
    bool EnableConsoleLog;
};
