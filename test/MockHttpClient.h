#pragma once
#include <gmock/gmock.h>
#include <functional>

#include "../src/IHttpClient.h"

class MockHttpClient : public IHttpClient 
{
public:
    MOCK_METHOD(bool, DownloadFile, (const std::string& hostName, const std::string& remotePath,
                                     std::function<bool(const std::string&, const size_t)> dataCallback));
};
