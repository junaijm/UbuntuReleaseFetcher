#pragma once

#include <string>

class ILogger
{
public:
    virtual void LogInfo(std::string logText) = 0;
    virtual void LogWarning(std::string logText) = 0;
    virtual void LogError(std::string logText) = 0;
};
