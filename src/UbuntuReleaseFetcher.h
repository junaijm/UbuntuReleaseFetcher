#pragma once
#include <memory>

#include "IReleaseFetcher.h"

// Forward declarations.
class ILogger;
class IHttpClient;
class UbuntuReleaseInfo;

class UbuntuReleaseFetcher : public IReleaseFetcher
{
public:
    UbuntuReleaseFetcher(const std::string& host,
                         const std::string& target, 
                         std::shared_ptr<ILogger> logger,
                         std::shared_ptr<IHttpClient> httpClient);
    virtual ~UbuntuReleaseFetcher();

    // Implement IImageFetcher methods
    bool GetSupportedVersions(const std::string& architecture, 
                              std::vector<std::string>& supportedVersions)      override;
    bool GetCurrentLTSRelease(const std::string& architecture, 
                              std::string& ltsRelease)                          override;
    bool GetPackageFileInfo(const std::string& versionName, 
                            const std::string& fileName, 
                            const std::string& infoTag, 
                            std::string& fileInfo)                              override;

private:
    std::shared_ptr<ILogger> Logger;
    std::shared_ptr<IHttpClient> HttpClient;
    std::shared_ptr<UbuntuReleaseInfo> ReleaseInfo;
};
