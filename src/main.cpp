#include <iostream>
#include <filesystem>
#include <boost/program_options.hpp>

#include "UbuntuReleaseFetcher.h"
#include "FileLogger.h"
#include "BoostHttpClient.h"

namespace BoostOptions = boost::program_options;

int main(int argc, char* argv[])
{
    BoostOptions::options_description cliDescription("Allowed options");
    cliDescription.add_options()
        ("help", "Displays help message")
        ("versions", "Print all supported Ubuntu versions for [amd64] architecture")
        ("checksum", BoostOptions::value<std::string>(), "Print checksum[sha256] of [disk1.img] for given release version")
        ("ltsrelease", "Print LTS release for [amd64] architecture")
        ("consolelog", "Enables logging on console");

    BoostOptions::variables_map argMap;
    try
    {
        BoostOptions::store(BoostOptions::parse_command_line(argc, argv, cliDescription), argMap);
        BoostOptions::notify(argMap);
    }
    catch (...)
    {
        // Something went wrong while parsing the input. Show help!
        std::cout << cliDescription << std::endl;
        return 0;
    }

    bool logToConsole = false;
    if (argMap.count("consolelog"))
    {
        logToConsole = true;
    }

    if (argMap.count("help")) 
    {
        std::cout << cliDescription << std::endl;
        return 0;
    }
    else if(argMap.count("versions") || argMap.count("checksum") || argMap.count("ltsrelease"))
    {
        // Initialize UbuntuReleaseFetcher. This is required for all commands.
        const std::string host = "cloud-images.ubuntu.com";
        const std::string target = "/releases/streams/v1/com.ubuntu.cloud:released:download.json";

        auto tempDir = std::filesystem::temp_directory_path();
        const std::string tempLogPath = tempDir.string() + "UbuntuReleaseFetcherLogs.txt";
        std::cout << "Initializing file logger with path [" << tempLogPath << "]" << std::endl;

        auto logger = std::make_shared<FileLogger>(tempLogPath, logToConsole);
        auto httpClient = std::make_shared<BoostHttpClient>(logger);

        UbuntuReleaseFetcher ubuntuReleaseFetcher(host, target, logger, httpClient);

        if (argMap.count("versions"))
        {
            std::vector<std::string> supportedVersions;
            // Though the fetcher supports querying supported versions for all architectures, 
            // application uses "GetSupportedVersions" for "amd64" architecture alone. Hence hardcoded the input.
            if (ubuntuReleaseFetcher.GetSupportedVersions("amd64", supportedVersions))
            {
                std::cout << "Supported versions for [amd64] achitectrue are:" << std::endl;
                for (auto version : supportedVersions)
                {
                    std::cout << " - " + version << std::endl;
                }
            }
        }
        else if (argMap.count("checksum"))
        {
            std::string versionName = argMap["checksum"].as<std::string>();
            std::string packageChecksum;

            // Though the fetcher supports querying of different file info, 
            // application uses "GetPackageFileInfo" for fetching "sha256" of "disk1.img". Hence hardcoded the input.
            if (ubuntuReleaseFetcher.GetPackageFileInfo(versionName,
                                                        "disk1.img",
                                                        "sha256", packageChecksum))
            {
                std::cout << "[sha256] of [disk1.img] of <" << versionName << "> is: " << packageChecksum << std::endl;
            }
        }
        else // if (argMap.count("ltsrelease"))
        {
            std::string ltsRelease;
            // Though the fetcher supports querying LTS for all architectures, 
            // application uses "GetCurrentLTSRelease" for "amd64" architecture alone. Hence hardcoded the input.
            if (ubuntuReleaseFetcher.GetCurrentLTSRelease("amd64", ltsRelease))
            {
                std::cout << "LTS release for [amd64] architecture is: " << ltsRelease << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Invalid input.!" << std::endl << cliDescription << std::endl;
    }

    return 0;
}
