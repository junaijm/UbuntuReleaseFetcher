#pragma once

#include <string>

class ILogger
{
public:
    virtual void LogInfo(const std::string& logText) = 0;
    virtual void LogWarning(const std::string& logText) = 0;
    virtual void LogError(const std::string& logText) = 0;
};
