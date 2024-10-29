#pragma once
#include "IHttpClient.h"

class BoostHttpClient : public IHttpClient
{
public:
    bool DownloadFile(const std::string& host, const std::string& remotePath, const std::string& downloadPath) override;
};

