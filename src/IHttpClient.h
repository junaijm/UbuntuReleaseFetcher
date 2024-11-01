#pragma once
#include <string>
#include <functional>

class IHttpClient 
{
public:
    virtual ~IHttpClient() = default;
    virtual bool DownloadFile(const std::string& hostName, const std::string& remotePath,
                              std::function<bool(const std::string&, const size_t)> dataCallback) = 0;
};
