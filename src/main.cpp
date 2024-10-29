#include <iostream>
#include <filesystem>

#include "UbuntuReleaseFetcher.h"
#include "FileLogger.h"
#include "BoostHttpClient.h"

int main() 
{
    const std::string host = "cloud-images.ubuntu.com";
    const std::string target = "/releases/streams/v1/com.ubuntu.cloud:released:download.json";

    auto tempDir = std::filesystem::temp_directory_path();
    const std::string tempLogPath = tempDir.string() + "UbuntuReleaseFetcherLogs.txt";
    std::cout << "Initializing file logger with path [" << tempLogPath << "]" << std::endl;

    auto logger = std::make_shared<FileLogger>(tempLogPath, true);
    auto httpClient = std::make_shared<BoostHttpClient>();

    UbuntuReleaseFetcher ubuntuReleaseFetcher(host, target, logger, httpClient);

    std::vector<std::string> supportedVersions;
    if (ubuntuReleaseFetcher.GetSupportedVersions("amd64", supportedVersions))
    {
        logger->LogInfo("Supported versions for [amd64] achitectrue are:");
        for (auto version : supportedVersions)
        {
            logger->LogInfo(version);
        }
    }

    std::string packageChecksum;
    if (ubuntuReleaseFetcher.GetPackageFileInfo("ubuntu-noble-24.04-amd64-server-20241004",
                                                "disk1.img",
                                                "sha256", packageChecksum))
    {
        logger->LogInfo("[sha256] of [disk1.img] of [ubuntu-noble-24.04-amd64-server-20241004] is: " + packageChecksum);
    }

    std::string ltsRelease;
    if (ubuntuReleaseFetcher.GetCurrentLTSRelease("amd64", ltsRelease))
    {
        logger->LogInfo("LTS release for [amd64] architecture is: " + ltsRelease);
    }

    return 0;
}
