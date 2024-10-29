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
        ("ltsRelease", "Print LTS release for [amd64] architecture");

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

    if (argMap.count("help")) 
    {
        std::cout << cliDescription << std::endl;
        return 0;
    }
    else if(argMap.count("versions") || argMap.count("checksum") || argMap.count("ltsRelease"))
    {
        // Initialize UbuntuReleaseFetcher. This is required for all commands.
        const std::string host = "cloud-images.ubuntu.com";
        const std::string target = "/releases/streams/v1/com.ubuntu.cloud:released:download.json";

        auto tempDir = std::filesystem::temp_directory_path();
        const std::string tempLogPath = tempDir.string() + "UbuntuReleaseFetcherLogs.txt";
        std::cout << "Initializing file logger with path [" << tempLogPath << "]" << std::endl;

        auto logger = std::make_shared<FileLogger>(tempLogPath, true);
        auto httpClient = std::make_shared<BoostHttpClient>();

        UbuntuReleaseFetcher ubuntuReleaseFetcher(host, target, logger, httpClient);

        if (argMap.count("versions"))
        {
            std::vector<std::string> supportedVersions;
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
            if (ubuntuReleaseFetcher.GetPackageFileInfo(versionName,
                                                        "disk1.img",
                                                        "sha256", packageChecksum))
            {
                std::cout << "[sha256] of [disk1.img] of <" << versionName << "> is: " << packageChecksum << std::endl;
            }
        }
        else // if (argMap.count("ltsRelease"))
        {
            std::string ltsRelease;
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
