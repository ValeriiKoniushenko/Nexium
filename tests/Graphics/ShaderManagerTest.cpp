// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Graphics/ShaderManager.h"

#include "gtest/gtest.h"

using namespace Core;

class ShaderManagerFixture : public ::testing::Test
{
protected:
    void SetUp() override { manager = &GetShaderManager(); }
    void TearDown() override
    {
        manager->getSuitableFragFileExtensions().clear();
        manager->getSuitableVertFileExtensions().clear();
        manager->pushSuitableFileExtension(".frag", ShaderType::Fragment);
        manager->pushSuitableFileExtension(".vert", ShaderType::Vertex);
    }

    ShaderManager* manager = nullptr;
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
    manager->pushSuitableFileExtension(extension, ShaderType::Fragment);
    auto& set = manager->getSuitableFragFileExtensions();
    EXPECT_TRUE(set.find(extension) != set.end());
}

TEST_F(ShaderManagerFixture, CanAddNewVertexExtension)
{
    const auto extension = ".vsh";
    manager->pushSuitableFileExtension(extension, ShaderType::Vertex);
    auto& set = manager->getSuitableVertFileExtensions();
    EXPECT_TRUE(set.find(extension) != set.end());
}

TEST_F(ShaderManagerFixture, IgnoresIncorrectShaderType)
{
    const auto extension = ".junk";
    manager->pushSuitableFileExtension(extension, static_cast<ShaderType>(999));
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
