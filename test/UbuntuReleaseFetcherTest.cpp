#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <memory>

#include "..\src\UbuntuReleaseFetcher.h"
#include "Mocks.h"

using namespace testing;

class UbuntuReleaseFetcherTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        // Setup code that runs before each test
        ::testing::FLAGS_gmock_verbose = "error";
    }

    void TearDown() override {
        // Cleanup code that runs after each test
        auto tempDir = std::filesystem::temp_directory_path();
        const std::string tempJsonPath = tempDir.string() + "UbuntuReleaseInfo.json";
        std::filesystem::remove(tempJsonPath);
    }

    const std::string Host = "cloud-images.ubuntu.com";
    const std::string Target = "/releases/streams/v1/com.ubuntu.cloud:released:download.json";
    const std::string TempJsonPath = std::filesystem::temp_directory_path().string() + "UbuntuReleaseInfo.json";
    const std::string TestDataDir = std::filesystem::current_path().string() + "\\testData\\";
};

TEST_F(UbuntuReleaseFetcherTest, LoadValidReleaseInfo) 
{
    auto mockHttpClient = std::make_shared<MockHttpClient>();
    auto mockLogger = std::make_shared<MockLogger>();

    EXPECT_CALL(*mockHttpClient, DownloadFile(Host, Target, TempJsonPath)).WillOnce(Invoke(
        [&]() -> bool
        {
            std::filesystem::copy_file(TestDataDir + "TD_LoadValidReleaseInfo.json", TempJsonPath,
                                       std::filesystem::copy_options::overwrite_existing);
            return true;
        }));

    std::shared_ptr<IReleaseFetcher> releaseFetcher = std::make_shared<UbuntuReleaseFetcher>
                                                      (Host, Target, mockLogger, mockHttpClient);

    std::vector<std::string> supportedVersions;
    EXPECT_TRUE(releaseFetcher->GetSupportedVersions("amd64", supportedVersions));
    EXPECT_EQ(supportedVersions.size(), 15);

    std::string ltsRelease;
    EXPECT_TRUE(releaseFetcher->GetCurrentLTSRelease("amd64", ltsRelease));
    EXPECT_EQ(ltsRelease, "24.04 LTS");

    std::string sha256;
    EXPECT_TRUE(releaseFetcher->GetPackageFileInfo("ubuntu-noble-24.04-amd64-server-20240423", "disk1.img", "sha256", sha256));
    EXPECT_EQ(sha256, "32a9d30d18803da72f5936cf2b7b9efcb4d0bb63c67933f17e3bdfd1751de3f3");
}

