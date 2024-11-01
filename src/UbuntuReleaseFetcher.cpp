#include <chrono>
#include <sstream>

#include "UbuntuReleaseFetcher.h"
#include "ILogger.h"
#include "IHttpClient.h"
#include "UbuntuReleaseInfo.h"

/// <summary>
/// Constructor.
/// </summary>
/// <param name="host">host name where Ubuntu release information is stored</param>
/// <param name="target">path to Ubuntu release information JSON</param>
/// <param name="logger">logger instance for diagnostic logging</param>
/// <param name="httpClient">http client instance to be used for HTTP GET</param>
UbuntuReleaseFetcher::UbuntuReleaseFetcher(
    const std::string& host,
    const std::string& target,
    std::shared_ptr<ILogger> logger,
    std::shared_ptr<IHttpClient> httpClient)
    : 
    Logger(logger),
    HttpClient(httpClient),
    ReleaseInfo(std::make_shared<UbuntuReleaseInfo>(logger))
{
    Logger->LogInfo("Fetching UbuntuReleaseInfo from [" + host + target + "]");

    // Download release information JSON and populate internal data structure for all supported versions.
    auto startOfDownload = std::chrono::high_resolution_clock::now();

    auto downloadStatus = ReleaseInfo->BeginParse();   
    downloadStatus = downloadStatus ? HttpClient->DownloadFile(host, target,
        [&](const std::string& fileData, const size_t dataSize) -> bool
        {
            return ReleaseInfo->ParseReleaseInfo(fileData, dataSize);
        }) : downloadStatus;
    downloadStatus = downloadStatus ? ReleaseInfo->EndParse() : downloadStatus;

    if (!downloadStatus)
    {
        Logger->LogError("Failed to download UbuntuReleaseInfo");
    }
    else
    {
        auto endOfDownload = std::chrono::high_resolution_clock::now();
        auto timeTakenForDownload = endOfDownload - startOfDownload;

        Logger->LogInfo("UbuntuReleaseInfo downloaded successfully.");

        std::stringstream perfData;
        perfData << "Time taken for downloading UbuntuReleaseInfo is : "
                 << std::chrono::duration_cast<std::chrono::milliseconds>(endOfDownload - startOfDownload).count() << " milliseconds";
        Logger->LogInfo(perfData.str());
    }
}

/// <summary>
/// Destructor
/// </summary>
UbuntuReleaseFetcher::~UbuntuReleaseFetcher()
{
}

/// <summary>
/// Function to fetch all supported Ubuntu version for a given architecture.
/// </summary>
/// <param name="architecture">architecture for which Ubuntu version are queried</param>
/// <param name="supportedVersions">OutParam: vector of supported version pubnames</param>
/// <returns>true, if successful</returns>
bool UbuntuReleaseFetcher::GetSupportedVersions(const std::string& architecture, 
                                                std::vector<std::string>& supportedVersions)
{
    return ReleaseInfo->GetSupportedVersions(architecture, supportedVersions);
}

/// <summary>
/// Function to fetch the Ubuntu LTS release for a given architecture, which has the longest support.
/// </summary>
/// <param name="architecture">architecture for which LTS release is quried</param>
/// <param name="ltsRelease">OutParam: LTS release title</param>
/// <returns>true, if successful</returns>
bool UbuntuReleaseFetcher::GetCurrentLTSRelease(const std::string& architecture, 
                                                std::string& ltsRelease)
{
    return ReleaseInfo->GetCurrentLTSRelease(architecture, ltsRelease);
}

/// <summary>
/// Function to return file info (such as checksum) of a given file in a given release version.
/// </summary>
/// <param name="versionName">pubname of the release version</param>
/// <param name="fileName">fileName of which info to be fetched</param>
/// <param name="infoTag">attribute of the file to be fetched (like "sha256")</param>
/// <param name="fileInfo">OutParam: Info of the file</param>
/// <returns>true, if successful</returns>
bool UbuntuReleaseFetcher::GetPackageFileInfo(const std::string& versionName, 
                                              const std::string& fileName, 
                                              const std::string& infoTag, 
                                              std::string& fileInfo)
{
    return ReleaseInfo->GetPackageFileInfo(versionName, fileName, infoTag, fileInfo);
}






