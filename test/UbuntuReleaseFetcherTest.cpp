#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <memory>
#include <fstream>

#include "..\src\UbuntuReleaseFetcher.h"
#include "MockHttpClient.h"
#include "MockLogger.h"

using namespace testing;

class UbuntuReleaseFetcherTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        // Setup code that runs before each test
        ::testing::FLAGS_gmock_verbose = "error";
    }

    void TearDown() override 
    {
        // Cleanup code that runs after each test
    }

    /// <summary>
    /// Helper function to read test data file (ReleaseInfo Json)
    /// </summary>
    /// <param name="filePath">full path to test data </param>
    /// <param name="dataCallback">callback function to send back the data read from file</param>
    /// <returns>true, if successful</returns>
    bool readFileInChunks(const std::string& filePath, std::function<bool(const std::string&, const size_t)> dataCallback)
    {
        std::ifstream fileToRead(filePath, std::ios::in | std::ios::binary);
        if (!fileToRead.is_open())
        {
            std::cerr << "Could not open file : " << filePath << std::endl;
            return false;
        }

        const int CHUNK_SIZE_FOR_READ = 10 * 1024; // 10 KB
        std::string readBuffer(CHUNK_SIZE_FOR_READ, '\0');

        while (fileToRead.read(&readBuffer[0], readBuffer.size()) || fileToRead.gcount() > 0)
        {
            if (!dataCallback(readBuffer, fileToRead.gcount()))
            {
                return false;
            }
        }

        return true;
    }

    const std::string Host = "cloud-images.ubuntu.com";
    const std::string Target = "/releases/streams/v1/com.ubuntu.cloud:released:download.json";
    const std::string TempJsonPath = std::filesystem::temp_directory_path().string() + "UbuntuReleaseInfo.json";
    const std::string TestDataDir = std::filesystem::current_path().string() + "\\testData\\";
};

TEST_F(UbuntuReleaseFetcherTest, LoadValidReleaseInfo) 
{
    auto mockLogger = std::make_shared<MockLogger>();
    auto mockHttpClient = std::make_shared<MockHttpClient>();

    mockHttpClient->DownloadFileMock = [&](const std::string& hostName,
        const std::string& remotePath,
        std::function<bool(const std::string&, const size_t)> dataCallback) -> bool
        {
            return readFileInChunks(TestDataDir + "TD_ValidReleaseInfo.json", dataCallback);
        };

    std::shared_ptr<IReleaseFetcher> releaseFetcher = std::make_shared<UbuntuReleaseFetcher>
                                                      (Host, Target, mockLogger, mockHttpClient);

    std::vector<std::string> supportedVersions;
    EXPECT_TRUE(releaseFetcher->GetSupportedVersions("amd64", supportedVersions));
    EXPECT_EQ(supportedVersions.size(), 234);

    std::string ltsRelease;
    EXPECT_TRUE(releaseFetcher->GetCurrentLTSRelease("amd64", ltsRelease));
    EXPECT_EQ(ltsRelease, "24.04 LTS");

    std::string sha256;
    EXPECT_TRUE(releaseFetcher->GetPackageFileInfo("ubuntu-noble-24.04-amd64-server-20240423", "disk1.img", "sha256", sha256));
    EXPECT_EQ(sha256, "32a9d30d18803da72f5936cf2b7b9efcb4d0bb63c67933f17e3bdfd1751de3f3");

    supportedVersions.clear();
    EXPECT_TRUE(releaseFetcher->GetSupportedVersions("i386", supportedVersions));
    EXPECT_EQ(supportedVersions.size(), 0);

    ltsRelease.clear();
    EXPECT_TRUE(releaseFetcher->GetCurrentLTSRelease("i386", ltsRelease));
    EXPECT_EQ(ltsRelease, "");

    supportedVersions.clear();
    EXPECT_TRUE(releaseFetcher->GetSupportedVersions("armhf", supportedVersions));
    EXPECT_EQ(supportedVersions.size(), 234);

    ltsRelease.clear();
    EXPECT_TRUE(releaseFetcher->GetCurrentLTSRelease("armhf", ltsRelease));
    EXPECT_EQ(ltsRelease, "24.04 LTS");

    sha256.clear();
    EXPECT_TRUE(releaseFetcher->GetPackageFileInfo("ubuntu-noble-24.04-armhf-server-20240423", "disk1.img", "sha256", sha256));
    EXPECT_EQ(sha256, "8713000ae4d230acda4edb8c1161dcd71ffb6861eac0837bd1b3b0d5fe402f60");
    
}

TEST_F(UbuntuReleaseFetcherTest, ReleaseInfoDownloadFailed)
{
    bool downloadFailureLoggedAsError = false;
    bool releaseInfoInitErrorLogged = false;
    auto mockLogger = std::make_shared<MockLogger>();
    ON_CALL(*mockLogger, LogError(_)).WillByDefault(Invoke(
        [&](const std::string& logText)
        {
            if (logText == "Failed to download UbuntuReleaseInfo")
            {
                downloadFailureLoggedAsError = true;
            }
            
            if (logText == "ReleaseInfo not initialized")
            {
                releaseInfoInitErrorLogged = true;
            }
        }));

    auto mockHttpClient = std::make_shared<MockHttpClient>();
    mockHttpClient->DownloadFileMock = [&](const std::string& hostName,
        const std::string& remotePath,
        std::function<bool(const std::string&, const size_t)> dataCallback) -> bool
        {
            return false;
        };

    std::shared_ptr<IReleaseFetcher> releaseFetcher = std::make_shared<UbuntuReleaseFetcher>
                                                      (Host, Target, mockLogger, mockHttpClient);
    EXPECT_TRUE(downloadFailureLoggedAsError);

    releaseInfoInitErrorLogged = false;
    std::vector<std::string> supportedVersions;
    EXPECT_FALSE(releaseFetcher->GetSupportedVersions("amd64", supportedVersions));
    EXPECT_TRUE(releaseInfoInitErrorLogged);

    releaseInfoInitErrorLogged = false;
    std::string ltsRelease;
    EXPECT_FALSE(releaseFetcher->GetCurrentLTSRelease("amd64", ltsRelease));
    EXPECT_TRUE(releaseInfoInitErrorLogged);

    releaseInfoInitErrorLogged = false;
    std::string sha256;
    EXPECT_FALSE(releaseFetcher->GetPackageFileInfo("ubuntu-noble-24.04-amd64-server-20240423", "disk1.img", "sha256", sha256));
    EXPECT_TRUE(releaseInfoInitErrorLogged);
}

TEST_F(UbuntuReleaseFetcherTest, InvalidReleaseInfoJson)
{
    bool downloadFailureLoggedAsError = false;
    bool releaseInfoInitErrorLogged = false;
    auto mockLogger = std::make_shared<MockLogger>();
    ON_CALL(*mockLogger, LogError(_)).WillByDefault(Invoke(
        [&](const std::string& logText)
        {
            if (logText == "Failed to download UbuntuReleaseInfo")
            {
                downloadFailureLoggedAsError = true;
            }

            if (logText == "ReleaseInfo not initialized")
            {
                releaseInfoInitErrorLogged = true;
            }
        }));

    auto mockHttpClient = std::make_shared<MockHttpClient>();

    mockHttpClient->DownloadFileMock = [&](const std::string& hostName,
        const std::string& remotePath,
        std::function<bool(const std::string&, const size_t)> dataCallback) -> bool
        {
            return readFileInChunks(TestDataDir + "TD_InvalidReleaseInfo.json", dataCallback);
        };

    std::shared_ptr<IReleaseFetcher> releaseFetcher = std::make_shared<UbuntuReleaseFetcher>
                                                      (Host, Target, mockLogger, mockHttpClient);

    EXPECT_TRUE(downloadFailureLoggedAsError);

    std::vector<std::string> supportedVersions;
    EXPECT_FALSE(releaseFetcher->GetSupportedVersions("amd64", supportedVersions));
    EXPECT_TRUE(releaseInfoInitErrorLogged);
}
