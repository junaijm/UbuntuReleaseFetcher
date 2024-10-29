#include <string>
#include <vector>

class IReleaseFetcher 
{
public:
    virtual ~IReleaseFetcher() = default;
    virtual bool GetSupportedVersions(const std::string& architecture, 
                                      std::vector<std::string>& supportedVersions) = 0;
    virtual bool GetCurrentLTSRelease(const std::string& architecture, 
                                      std::string& ltsRelease) = 0;
    virtual bool GetPackageFileInfo(const std::string& versionName, 
                                    const std::string& fileName, 
                                    const std::string& infoTag, 
                                    std::string& fileInfo) = 0;
};
