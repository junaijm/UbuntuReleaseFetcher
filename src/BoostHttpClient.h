#pragma once
#include <memory>
#include "IHttpClient.h"

class ILogger; // Forward declaration.

class BoostHttpClient : public IHttpClient
{
public:
    BoostHttpClient(std::shared_ptr<ILogger> logger);
    virtual ~BoostHttpClient();
    bool DownloadFile(const std::string& hostName, const std::string& remotePath,
                      std::function<bool(const std::string&, const size_t)> dataCallback)       override;

private:
    std::shared_ptr<ILogger> Logger;
};

