#pragma once
#include <gmock/gmock.h>

#include "../src/ILogger.h"

class MockLogger : public ILogger
{
public:
    MOCK_METHOD(void, LogInfo, (const std::string& logText));
    MOCK_METHOD(void, LogWarning, (const std::string& logText));
    MOCK_METHOD(void, LogError, (const std::string& logText));
};