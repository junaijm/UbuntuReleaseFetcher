#pragma once
#include <gmock/gmock.h>

#include "../src/IHttpClient.h"
#include "../src/ILogger.h"

class MockHttpClient : public IHttpClient 
{
public:
    MOCK_METHOD(bool, DownloadFile, (const std::string& host, const std::string& remotePath, const std::string& downloadPath));
};

class MockLogger : public ILogger
{
public:
    MOCK_METHOD(void, LogInfo, (const std::string& logText));
    MOCK_METHOD(void, LogWarning, (const std::string& logText));
    MOCK_METHOD(void, LogError, (const std::string& logText));
};