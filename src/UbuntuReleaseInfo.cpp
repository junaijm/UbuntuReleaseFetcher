#include <iostream>
#include <fstream>

#include "UbuntuReleaseInfo.h"
#include "ILogger.h"

namespace json = boost::json;

/// <summary>
/// Constructor.
/// </summary>
/// <param name="logger">Logger instance to be used for diagnostic logging</param>
UbuntuReleaseInfo::UbuntuReleaseInfo(std::shared_ptr<ILogger> logger) : Logger(logger)
{
}

/// <summary>
/// Destructor
/// </summary>
UbuntuReleaseInfo::~UbuntuReleaseInfo()
{
}

/// <summary>
/// Function to fill internal data structure with Ubuntu release information.
/// </summary>
/// <param name="releaseInfoFile">Source file where release information is stored</param>
/// <returns>true, if successful</returns>
bool UbuntuReleaseInfo::LoadReleaseInfoFromFile(const std::string& releaseInfoFile)
{
    boost::json::value releaseInfoJson;
    if (!loadReleaseInfoFileToJson(releaseInfoFile, releaseInfoJson) || 
        !populateSupportedReleases(releaseInfoJson))
    {
        Logger->LogError("Failed to parse UbuntuReleaseInfoFile. ");
        return false;
    }

    return true;
}

/// <summary>
/// Function to fetch all supported Ubuntu versions for a given processor architecture.
/// </summary>
/// <param name="architecture">target architecture. "*" means all architectures</param>
/// <param name="supportedVersions">OutParam: vector of supported Ubuntu version pubnames</param>
/// <returns>true, if successful</returns>
bool UbuntuReleaseInfo::GetSupportedVersions(const std::string& architecture, std::vector<std::string>& supportedVersions)
{
    try
    {
        for (auto const& supportedRelease : SupportedReleases)
        {
            if (architecture == "*" || supportedRelease.architecture == architecture)
            {
                for (auto const& version : supportedRelease.versions)
                {
                    supportedVersions.push_back(version.pubName);
                }
            }
        }
    }
    catch (const std::exception& exceptionObj)
    {
        Logger->LogError("Exception caught in UbuntuReleaseInfo::GetSupportedVersions.");
        Logger->LogError("Exception text: " + std::string(exceptionObj.what()));
        return false;
    }

    return true;
}

/// <summary>
/// Function to fetch the Ubuntu LTS release for a given architecture, which has the longest support.
/// </summary>
/// <param name="architecture">architecture for which LTS release is quried</param>
/// <param name="ltsRelease">OutParam: LTS release title</param>
/// <returns>true, if successful</returns>
bool UbuntuReleaseInfo::GetCurrentLTSRelease(const std::string& architecture, std::string& ltsRelease)
{
    try
    {
        // Filter the LTS releaess for given architecture.
        // Filtering is done based on the releaseTitle.
        decltype(SupportedReleases) filteredVector;
        std::copy_if(SupportedReleases.begin(), SupportedReleases.end(), std::back_inserter(filteredVector), [&](const ProductInfo& releaseInfo)
            {
                return (architecture == releaseInfo.architecture && std::string::npos != releaseInfo.releaseTitle.find("LTS"));
            });

        // Sort (in Ascending order) the filtered vector to find longest supported LTS release.
        std::sort(filteredVector.begin(), filteredVector.end(), [&](const ProductInfo& lhs, const ProductInfo& rhs)
            {
                return dateStringToComparableInt(lhs.endOfSupport) > dateStringToComparableInt(rhs.endOfSupport);
            });

        ltsRelease = filteredVector[0].releaseTitle;
    }
    catch (const std::exception& exceptionObj)
    {
        Logger->LogError("Exception caught in UbuntuReleaseInfo::GetCurrentLTSRelease.");
        Logger->LogError("Exception text: " + std::string(exceptionObj.what()));
        return false;
    }

    return true;
}

/// <summary>
/// Function to return file info (such as checksum) of a given file in a given release version.
/// 
/// Important note: Querying of "sha256" alone is supported at the moment.
/// 
/// </summary>
/// <param name="versionName">pubname of the release version</param>
/// <param name="fileName">fileName of which info to be fetched</param>
/// <param name="infoTag">attribute of the file to be fetched (like "sha256")</param>
/// <param name="fileInfo">OutParam: Info of the file</param>
/// <returns>true, if successful</returns>
bool UbuntuReleaseInfo::GetPackageFileInfo(const std::string& versionName, const std::string& fileName, const std::string& infoTag, std::string& fileInfo)
{
    try
    {
        VersionInfo versionToFind{ versionName, {} };
        FileInfo fileToFind{ fileName, "" };
        for (auto const& supportedRelease : SupportedReleases)
        {
            auto versionIterator = std::find(supportedRelease.versions.begin(), supportedRelease.versions.end(), versionToFind);
            if (supportedRelease.versions.end() == versionIterator)
            {
                continue;
            }

            auto fileIterator = std::find(versionIterator->files.begin(), versionIterator->files.end(), fileToFind);
            if (versionIterator->files.end() == fileIterator)
            {
                Logger->LogError("Failed to find file info for " + fileName);
                return false;
            }

            if ("sha256" == infoTag)
            {
                fileInfo = fileIterator->sha256;
                return true;
            }
            else
            {
                Logger->LogWarning("Querying of file info {" + infoTag + "} is not supported at the moment.");
                return false;
            }
        }

        // Iteration completed without finding requested version.
        Logger->LogError("Failed to find version info for " + versionName);
        return false;
    }
    catch (const std::exception& exceptionObj)
    {
        Logger->LogError("Exception caught in UbuntuReleaseInfo::GetPackageFileInfo.");
        Logger->LogError("Exception text: " + std::string(exceptionObj.what()));
        return false;
    }

    // Could not fetch the package info for given input
    return false;
}

/// <summary>
/// Function to load Ubuntu release information file in to a JSON object.
/// Function uses stream_parser from Boost::json library for parsing JSON file.
/// </summary>
/// <param name="releaseInfoFile">Source file path where JSON data is stored</param>
/// <param name="jsonObj">OutParam: Resultant JSON object</param>
/// <returns>true, if successful</returns>
bool UbuntuReleaseInfo::loadReleaseInfoFileToJson(const std::string& releaseInfoFile, boost::json::value& jsonObj)
{
    try
    {
        std::ifstream imageInfoFile(releaseInfoFile, std::ios::in | std::ios::binary);
        if (!imageInfoFile.is_open())
        {
            Logger->LogError("Could not open releaseInfoFile : " + releaseInfoFile);
            return false;
        }

        const int CHUNK_SIZE_FOR_PARSING = 18 * 1024; // 10 KB
        std::string readBuffer(CHUNK_SIZE_FOR_PARSING, '\0');

        json::stream_parser jsonParser;
        while (imageInfoFile.read(&readBuffer[0], readBuffer.size()) || imageInfoFile.gcount() > 0)
        {
            jsonParser.write(readBuffer.data(), imageInfoFile.gcount()); // Pass data to parser in chunks
        }

        jsonParser.finish();
        if (jsonParser.done())
        {
            jsonObj = jsonParser.release(); // Retrieve JSON object from parser.
        }
        jsonParser.reset(); // Reset parser and release memory.
    }
    catch (const boost::system::system_error& boostException)
    {
        Logger->LogError("BoostException caught in UbuntuReleaseInfo::loadReleaseInfoFileToJson.");
        Logger->LogError("Exception text: " + std::string(boostException.what()));
        return false;
    }
    catch (const std::exception& exceptionObj)
    {
        Logger->LogError("Exception caught in UbuntuReleaseInfo::loadReleaseInfoFileToJson.");
        Logger->LogError("Exception text: " + std::string(exceptionObj.what()));
        return false;
    }

    return true;
}

/// <summary>
/// Function to iterate through JSON object and populate internal data structure for all supported Ubuntu versions.
/// Function skips the versions that are already out of support.
/// 
/// Assumption: Function assumes that JSON data adhere to Simplestream format and all required feilds are available.
///             Hence validation of input data is not performed.
/// </summary>
/// <param name="releaseInfoJson">JSON object of all available Ubuntu releases</param>
/// <returns>true, if successful</returns>
bool UbuntuReleaseInfo::populateSupportedReleases(boost::json::value releaseInfoJson)
{
    try
    {
        auto const& rootObj = releaseInfoJson.as_object();
        auto const& products = rootObj.at("products").as_object();
        // Iterate through each product
        for (auto const& product : products)
        {
            auto const& productObj = product.value().as_object();
            if (productObj.at("supported").as_bool())
            {
                ProductInfo productInfo;
                productInfo.architecture = productObj.at("arch").as_string().data();
                productInfo.releaseTitle = productObj.at("release_title").as_string().data();
                productInfo.endOfSupport = productObj.at("support_eol").as_string().data();

                auto const& versions = productObj.at("versions").as_object();
                for (auto const& version : versions)
                {
                    auto const& versionObj = version.value().as_object();

                    VersionInfo packageVersion;
                    packageVersion.pubName = versionObj.at("pubname").as_string().data();

                    auto const& items = versionObj.at("items").as_object();
                    for (auto const& item : items)
                    {
                        auto const& itemObj = item.value().as_object();

                        FileInfo packageFileInfo{
                            itemObj.at("ftype").as_string().data(),
                            itemObj.at("sha256").as_string().data()
                        };

                        packageVersion.files.push_back(packageFileInfo);
                    }

                    productInfo.versions.push_back(packageVersion);
                }

                SupportedReleases.push_back(productInfo);
            }
        }
    }
    catch (const std::exception& exceptionObj)
    {
        Logger->LogError("Exception caught in UbuntuReleaseInfo::populateSupportedReleases.");
        Logger->LogError("Exception text: " + std::string(exceptionObj.what()));
        return false;
    }

    return true;
}

/// <summary>
/// Utility function to convert dateString in YYYY-MM-DD format in to comparable integer YYYYMMDD
/// </summary>
/// <param name="dateString">date as string</param>
/// <returns>date as integer</returns>
int UbuntuReleaseInfo::dateStringToComparableInt(const std::string& dateString)
{
    // Remove dashes and convert to an integer. YYYY-MM-DD => int(YYYYMMDD)
    std::string dateNumStr = dateString.substr(0, 4) + dateString.substr(5, 2) + dateString.substr(8, 2);
    return std::stoi(dateNumStr);
}