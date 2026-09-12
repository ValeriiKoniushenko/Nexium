// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxSubsystems/Graphics/ShaderManager.h"

#include "Foundation/Configs.h"
#include "spdlog/spdlog.h"

#include "gtest/gtest.h"

using namespace Core;
using namespace NX;

class ShaderManagerFixture : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override
    {
        manager.getSuitableFragFileExtensions().clear();
        manager.getSuitableVertFileExtensions().clear();
        manager.pushSuitableFileExtension(".frag", ShaderType::Fragment);
        manager.pushSuitableFileExtension(".vert", ShaderType::Vertex);
    }

    ShaderManager& manager = ShaderManager::Instance();
};

TEST_F(ShaderManagerFixture, TestAssetsDirectoryIsNotEmpty)
{
    namespace fs = std::filesystem;
    const auto testAssetsDir = Foundation::Config::Path::projectAbsPath / "tests/assets/shaders";

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
    manager.pushSuitableFileExtension(extension, ShaderType::Fragment);
    auto& set = manager.getSuitableFragFileExtensions();
    EXPECT_TRUE(set.find(extension) != set.end());
}

TEST_F(ShaderManagerFixture, CanAddNewVertexExtension)
{
    const auto extension = ".vsh";
    manager.pushSuitableFileExtension(extension, ShaderType::Vertex);
    auto& set = manager.getSuitableVertFileExtensions();
    EXPECT_TRUE(set.find(extension) != set.end());
}

TEST_F(ShaderManagerFixture, IgnoresIncorrectShaderType)
{
    const auto extension = ".junk";
    manager.pushSuitableFileExtension(extension, static_cast<ShaderType>(999));
    auto& fragSet = manager.getSuitableFragFileExtensions();
    auto& vertSet = manager.getSuitableVertFileExtensions();
    EXPECT_TRUE(fragSet.find(extension) == fragSet.end());
    EXPECT_TRUE(vertSet.find(extension) == vertSet.end());
}

TEST_F(ShaderManagerFixture, ReturnsEmptyShaderProgramIfNotFound)
{
    auto shader = manager.getShaderProgram("non_existent_shader"_atom);

    EXPECT_EQ(shader, nullptr);
}
