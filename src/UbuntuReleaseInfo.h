#pragma once
#include <boost/json.hpp>

#include <string>

// Structure to hold important release informations.
struct FileInfo
{
    std::string fileType;
    std::string sha256;

    // Compare operator for enabling find in std::vector. 
    bool operator==(const FileInfo& rhs) const 
    {
        return (fileType == rhs.fileType);
    }
};

struct VersionInfo
{
    std::string pubName;
    std::vector<FileInfo> files;

    // Compare operator for enabling find in std::vector
    bool operator==(const VersionInfo& rhs) const
    {
        return (pubName == rhs.pubName);
    }
};

struct ProductInfo
{
    std::string architecture;
    std::string releaseTitle;
    std::string endOfSupport;
    std::vector<VersionInfo> versions;
};

class ILogger;
class UbuntuReleaseInfo
{
public:

    UbuntuReleaseInfo(std::shared_ptr<ILogger> logger);
    virtual ~UbuntuReleaseInfo();

    bool LoadReleaseInfoFromFile(const std::string& releaseInfoFile);
    bool GetSupportedVersions(const std::string& architecture, std::vector<std::string>& supportedVersions);
    bool GetCurrentLTSRelease(const std::string& architecture, std::string& ltsRelease);
    bool GetPackageFileInfo(const std::string& versionName, const std::string& fileName, const std::string& infoTag, std::string& fileInfo);

private:
    bool loadReleaseInfoFileToJson(const std::string& releaseInfoFile, boost::json::value& jsonObj);
    bool populateSupportedReleases(boost::json::value jsonObj);
    int dateStringToComparableInt(const std::string& dateString);

private:
    std::shared_ptr<ILogger> Logger;
    std::vector<ProductInfo> SupportedReleases;
};