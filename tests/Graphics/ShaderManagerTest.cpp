#include "Graphics/ShaderManager.h"

#include "gtest/gtest.h"

class ShaderManagerFixture : public ::testing::Test
{
protected:
    void SetUp() override { manager = &SW::GetShaderManager(); }
    void TearDown() override
    {
        manager->getSuitableFragFileExtensions().clear();
        manager->getSuitableVertFileExtensions().clear();
        manager->pushSuitableFileExtension(".frag", SW::ShaderType::Fragment);
        manager->pushSuitableFileExtension(".vert", SW::ShaderType::Vertex);
    }

    SW::ShaderManager* manager = nullptr;
};

TEST_F(ShaderManagerFixture, TestAssetsDirectoryIsNotEmpty)
{
    namespace fs = std::filesystem;
    const auto testAssetsDir = fs::path("tests/assets/shaders");

    if (!fs::exists(testAssetsDir))
    {
        spdlog::error("Directory does not exist: {}", testAssetsDir.string());
        FAIL() << "Directory does not exist";
    }

    if (!fs::is_directory(testAssetsDir))
    {
        spdlog::error("Path is not a directory: {}", testAssetsDir.string());
        FAIL() << "Path is not a directory";
    }

    bool hasFiles = false;
    for (const auto& entry : fs::directory_iterator(testAssetsDir))
    {
        if (fs::is_regular_file(entry))
        {
            hasFiles = true;
            break;
        }
    }

    if (!hasFiles)
    {
        spdlog::warn("Directory '{}' is empty", testAssetsDir.string());
    }

    EXPECT_TRUE(hasFiles) << "Test assets directory is empty: " << testAssetsDir;
}

TEST_F(ShaderManagerFixture, CanAddNewFragmentExtension)
{
    const auto extension = ".fsh";
    manager->pushSuitableFileExtension(extension, SW::ShaderType::Fragment);
    auto& set = manager->getSuitableFragFileExtensions();
    EXPECT_TRUE(set.find(extension) != set.end());
}

TEST_F(ShaderManagerFixture, CanAddNewVertexExtension)
{
    const auto extension = ".vsh";
    manager->pushSuitableFileExtension(extension, SW::ShaderType::Vertex);
    auto& set = manager->getSuitableVertFileExtensions();
    EXPECT_TRUE(set.find(extension) != set.end());
}

TEST_F(ShaderManagerFixture, IgnoresIncorrectShaderType)
{
    const auto extension = ".junk";
    manager->pushSuitableFileExtension(extension, static_cast<SW::ShaderType>(999));
    auto& fragSet = manager->getSuitableFragFileExtensions();
    auto& vertSet = manager->getSuitableVertFileExtensions();
    EXPECT_TRUE(fragSet.find(extension) == fragSet.end());
    EXPECT_TRUE(vertSet.find(extension) == vertSet.end());
}

TEST_F(ShaderManagerFixture, ReturnsEmptyShaderProgramIfNotFound)
{
    auto shader = manager->getShaderProgram("non_existent_shader"_atom);

    EXPECT_EQ(shader, nullptr);
}
