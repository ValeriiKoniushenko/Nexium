// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "AtomicFile.h"

#include <atomic>
#include <cerrno>
#include <chrono>
#include <fstream>
#include <limits>

#ifdef _WIN32
    #include <Windows.h>
#endif

namespace
{
    struct PendingFile
    {
        PendingFile() = default;
        PendingFile(const PendingFile&) = delete;
        PendingFile& operator=(const PendingFile&) = delete;
        PendingFile(PendingFile&&) = delete;
        PendingFile& operator=(PendingFile&&) = delete;

        ~PendingFile()
        {
            if (stream.is_open())
            {
                stream.close();
            }
            if (owned)
            {
                std::error_code ignored;
                std::filesystem::remove(path, ignored);
            }
        }

        std::filesystem::path path;
        std::ofstream stream;
        bool owned = false;
    };
} // namespace

namespace NX
{
    void WriteFileAtomically(const std::filesystem::path& path, std::string_view contents)
    {
        if (contents.size() > static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max()))
        {
            throw std::filesystem::filesystem_error(
                "File is too large", path, std::make_error_code(std::errc::file_too_large));
        }

        static std::atomic_uint64_t sequence{ 0 };
        PendingFile temporary;
        for (int attempt = 0; attempt < 64; ++attempt)
        {
            temporary.path
                = path.parent_path()
                  / (".nexium-save-"
                     + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())
                     + "-" + std::to_string(sequence.fetch_add(1)) + ".tmp");
            errno = 0;
            temporary.stream.open(temporary.path, std::ios::binary | std::ios::noreplace);
            if (temporary.stream.is_open())
            {
                temporary.owned = true;
                break;
            }
            if (errno != EEXIST)
            {
                const auto error = errno ? std::error_code(errno, std::generic_category())
                                         : std::make_error_code(std::errc::io_error);
                throw std::filesystem::filesystem_error("Cannot create temporary file", path,
                                                        error);
            }
            temporary.stream.clear();
        }
        if (!temporary.owned)
        {
            throw std::filesystem::filesystem_error("Cannot create unique temporary file", path,
                                                    std::make_error_code(std::errc::file_exists));
        }

        temporary.stream.write(contents.data(), static_cast<std::streamsize>(contents.size()));
        temporary.stream.flush();
        temporary.stream.close();
        if (!temporary.stream)
        {
            throw std::filesystem::filesystem_error("Cannot write temporary file", path,
                                                    std::make_error_code(std::errc::io_error));
        }

#ifdef _WIN32
        if (!MoveFileExW(temporary.path.c_str(), path.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        {
            throw std::filesystem::filesystem_error(
                "Cannot replace file", path,
                std::error_code(static_cast<int>(GetLastError()), std::system_category()));
        }
#else
        std::filesystem::rename(temporary.path, path);
#endif
        temporary.owned = false;
    }
} // namespace NX
