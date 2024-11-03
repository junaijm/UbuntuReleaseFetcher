#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <memory>
#include <fstream>

#include "../src/UbuntuReleaseFetcher.h"
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
    const std::string TestDataDir = std::filesystem::current_path().string() + "/testData/";
};

TEST_F(UbuntuReleaseFetcherTest, LoadValidReleaseInfo) 
{
    auto mockLogger = std::make_shared<MockLogger>();
    auto mockHttpClient = std::make_shared<MockHttpClient>();

    EXPECT_CALL(*mockHttpClient, DownloadFile(Host,Target,_)).WillRepeatedly(Invoke(
        [&](auto host, auto targer, auto dataCallback) -> bool
        {
            return readFileInChunks(TestDataDir + "TD_ValidReleaseInfo.json", dataCallback);
        }));

    std::shared_ptr<IReleaseFetcher> releaseFetcher = std::make_shared<UbuntuReleaseFetcher>
                                                      (Host, Target, mockLogger, mockHttpClient);

    std::vector<std::string> supportedVersions;
    EXPECT_TRUE(releaseFetcher->GetSupportedVersions("amd64", supportedVersions));
    EXPECT_EQ(supportedVersions.size(), 3);

    std::string ltsRelease;
    EXPECT_TRUE(releaseFetcher->GetCurrentLTSRelease("amd64", ltsRelease));
    EXPECT_EQ(ltsRelease, "24.04 LTS");

    std::string sha256;
    EXPECT_TRUE(releaseFetcher->GetPackageFileInfo("ubuntu-noble-24.04-amd64-server-20241004", "disk1.img", "sha256", sha256));
    EXPECT_EQ(sha256, "fad101d50b06b26590cf30542349f9e9d3041ad7929e3bc3531c81ec27f2c788");

    supportedVersions.clear();
    EXPECT_TRUE(releaseFetcher->GetSupportedVersions("i386", supportedVersions));
    EXPECT_EQ(supportedVersions.size(), 0);

    ltsRelease.clear();
    EXPECT_TRUE(releaseFetcher->GetCurrentLTSRelease("i386", ltsRelease));
    EXPECT_EQ(ltsRelease, "");

    supportedVersions.clear();
    EXPECT_TRUE(releaseFetcher->GetSupportedVersions("arm64", supportedVersions));
    EXPECT_EQ(supportedVersions.size(), 3);

    supportedVersions.clear();
    EXPECT_TRUE(releaseFetcher->GetSupportedVersions("*", supportedVersions));
    EXPECT_EQ(supportedVersions.size(), 9);

    sha256.clear();
    EXPECT_TRUE(releaseFetcher->GetPackageFileInfo("ubuntu-noble-24.04-s390x-server-20241004", "disk1.img", "sha256", sha256));
    EXPECT_EQ(sha256, "73eee05f6775a02d63f01c7745c17e39711eb076ab9a7c88b90bd95622d697d0");

    std::string md5;
    EXPECT_FALSE(releaseFetcher->GetPackageFileInfo("ubuntu-noble-24.04-s390x-server-20241004", "disk1.img", "md5", md5));
    EXPECT_TRUE(mockLogger->IsLogPresent("Querying of file info (md5) is not supported at the moment."));

}

TEST_F(UbuntuReleaseFetcherTest, ReleaseInfoDownloadFailed)
{
    auto mockLogger = std::make_shared<MockLogger>();

    auto mockHttpClient = std::make_shared<MockHttpClient>();
    EXPECT_CALL(*mockHttpClient, DownloadFile(Host, Target, _)).WillRepeatedly(Invoke(
        [&](auto host, auto targer, auto dataCallback) -> bool
        {
            return false;
        }));

    std::shared_ptr<IReleaseFetcher> releaseFetcher = std::make_shared<UbuntuReleaseFetcher>
                                                      (Host, Target, mockLogger, mockHttpClient);

    EXPECT_TRUE(mockLogger->IsLogPresent("Failed to download UbuntuReleaseInfo"));

    mockLogger->ClearLogs();
    std::vector<std::string> supportedVersions;
    EXPECT_FALSE(releaseFetcher->GetSupportedVersions("amd64", supportedVersions));
    EXPECT_TRUE(mockLogger->IsLogPresent("ReleaseInfo not initialized"));

    mockLogger->ClearLogs();
    std::string ltsRelease;
    EXPECT_FALSE(releaseFetcher->GetCurrentLTSRelease("amd64", ltsRelease));
    EXPECT_TRUE(mockLogger->IsLogPresent("ReleaseInfo not initialized"));

    mockLogger->ClearLogs();
    std::string sha256;
    EXPECT_FALSE(releaseFetcher->GetPackageFileInfo("ubuntu-noble-24.04-amd64-server-20240423", "disk1.img", "sha256", sha256));
    EXPECT_TRUE(mockLogger->IsLogPresent("ReleaseInfo not initialized"));
}

TEST_F(UbuntuReleaseFetcherTest, InvalidReleaseInfoJson)
{
    auto mockLogger = std::make_shared<MockLogger>();

    auto mockHttpClient = std::make_shared<MockHttpClient>();
    EXPECT_CALL(*mockHttpClient, DownloadFile(Host,Target,_)).WillRepeatedly(Invoke(
        [&](auto host, auto targer, auto dataCallback) -> bool
        {
            return readFileInChunks(TestDataDir + "TD_InvalidReleaseInfo.json", dataCallback);
        }));

    std::shared_ptr<IReleaseFetcher> releaseFetcher = std::make_shared<UbuntuReleaseFetcher>
                                                      (Host, Target, mockLogger, mockHttpClient);

    EXPECT_TRUE(mockLogger->IsLogPresent("Failed to download UbuntuReleaseInfo"));

    mockLogger->ClearLogs();
    std::vector<std::string> supportedVersions;
    EXPECT_FALSE(releaseFetcher->GetSupportedVersions("amd64", supportedVersions));
    EXPECT_TRUE(mockLogger->IsLogPresent("ReleaseInfo not initialized"));
}
