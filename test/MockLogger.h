#pragma once
#include <gmock/gmock.h>

#include "../src/ILogger.h"

class MockLogger : public ILogger
{
public:
    void LogInfo(const std::string& logText)
    {
        LogsCollected.push_back(logText);
    }

    void LogWarning(const std::string& logText)
    {
        LogsCollected.push_back(logText);
    }

    void LogError(const std::string& logText)
    {
        LogsCollected.push_back(logText);
    }

    bool IsLogPresent(const std::string& logToSearch)
    {
        return(LogsCollected.end() != std::find(LogsCollected.begin(), LogsCollected.end(), logToSearch));
    }

    void ClearLogs()
    {
        LogsCollected.clear();
    }

private:
    std::vector<std::string> LogsCollected;
};