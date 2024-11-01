#pragma once
#include <gmock/gmock.h>
#include <functional>

#include "../src/IHttpClient.h"

/// <summary>
/// GMock cannot be used to mock DownloadFile as it accepts a non-comparable std::function as parameter 
/// EXPECT_CALL macro fails to setup mock.
/// </summary>
class MockHttpClient : public IHttpClient 
{
public:
    bool DownloadFile(const std::string& hostName, const std::string& remotePath,
                      std::function<bool(const std::string&, const size_t)> dataCallback) override
    {
        if(nullptr != DownloadFileMock)
        { 
            return DownloadFileMock(hostName, remotePath, dataCallback);
        }
        else
        {
            return false;
        }
    }

    // Helper member for setting-up mock object.
    std::function<bool(const std::string& hostName, const std::string& remotePath,
                       std::function<bool(const std::string&, const size_t)> dataCallback)> DownloadFileMock;
};
