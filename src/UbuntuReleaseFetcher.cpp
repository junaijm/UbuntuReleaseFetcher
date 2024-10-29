#include <iostream>
#include <fstream>
#include <filesystem>

#include "UbuntuReleaseFetcher.h"
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
    HostName(host),
    TargetPath(target),
    Logger(logger),
    HttpClient(httpClient),
    ReleaseInfo(std::make_shared<UbuntuReleaseInfo>(logger))
{
    // Download release information JSON to local system for parsing.
    // Use temp folder as download destination.
    auto tempDir = std::filesystem::temp_directory_path();
    const std::string tempJsonPath = tempDir.string() + "UbuntuReleaseInfo.json";
    Logger->LogInfo("Downloading UbuntuReleaseInfo to " + tempJsonPath);
    
    if (HttpClient->DownloadFile(HostName, TargetPath, tempJsonPath))
    {
        if (!ReleaseInfo->LoadReleaseInfoFromFile(tempJsonPath))
        {
            Logger->LogError("Failed to parse UbuntuReleaseInfo");
        }
    }
    else
    {
        Logger->LogError("Failed to download UbuntuReleaseInfo");
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






