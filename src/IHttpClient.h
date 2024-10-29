#pragma once
#include <string>

class IHttpClient 
{
public:
    virtual ~IHttpClient() = default;
    virtual bool DownloadFile(const std::string& host, const std::string& remotePath, const std::string& downloadPath) = 0;
};
