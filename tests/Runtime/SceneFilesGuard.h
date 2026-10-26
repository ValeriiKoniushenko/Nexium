// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Foundation/Configs.h"

#include <filesystem>
#include <set>

// SceneManager writes scene files into the real data directory. Removes the files a test
// created there, so test runs do not leave scenes behind.
class SceneFilesGuard final
{
public:
    SceneFilesGuard()
        : _existing(listFiles())
    {
    }

    SceneFilesGuard(const SceneFilesGuard&) = delete;
    SceneFilesGuard& operator=(const SceneFilesGuard&) = delete;

    ~SceneFilesGuard()
    {
        for (const auto& path : listFiles())
        {
            if (!_existing.contains(path))
            {
                std::error_code error;
                std::filesystem::remove(path, error);
            }
        }
    }

private:
    [[nodiscard]] static std::set<std::filesystem::path> listFiles()
    {
        std::set<std::filesystem::path> files;
        std::error_code error;
        for (const auto& entry : std::filesystem::directory_iterator(
                 Foundation::Config::Path::data / "scenes", error))
        {
            files.insert(entry.path());
        }
        return files;
    }

    std::set<std::filesystem::path> _existing;
};
