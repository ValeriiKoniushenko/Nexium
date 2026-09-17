// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxFundamental/Assets/BaseAsset.h"
#include "NxFundamental/ResourceManagement/AtomicFile.h"
#include "NxFundamental/ResourceManagement/DataStream.h"

#include "gtest/gtest.h"
#include <chrono>
#include <fstream>
#include <thread>

#ifndef _WIN32
    #include <csignal>
    #include <sys/resource.h>
    #include <sys/wait.h>
    #include <unistd.h>
#endif

namespace
{
    class AtomicFileTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            root
                = std::filesystem::temp_directory_path()
                  / ("nexium_atomic_file_tests_"
                     + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
            std::filesystem::create_directories(root);
        }
        void TearDown() override { std::filesystem::remove_all(root); }

        static std::string read(const std::filesystem::path& path)
        {
            std::ifstream stream(path, std::ios::binary);
            return { std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>() };
        }

        std::filesystem::path root;
    };

    struct TestDocument : Foundation::IDataIO
    {
        std::filesystem::path directory;
        int value = 0;
        void deserialize(RResourceStream<RJsonResourceStream>& data)
        {
            value = data.getData().at("value").get<int>();
        }
        [[nodiscard]] std::filesystem::path getCacheDir() const override { return directory; }
        [[nodiscard]] Core::StringAtom getCacheHash() const override { return "document"_atom; }
    };

    class TestAsset final : public NX::BaseAsset
    {
    public:
        TestAsset()
            : BaseAsset("test"_atom)
        {
        }

    protected:
        void onLoadRequest() override {}
        void onUnloadRequest() override {}
        void ioFieldsUpdate(NX::DataStream& out) override { out.getRaw()["updated"] = true; }
    };
} // namespace

TEST_F(AtomicFileTest, CreatesAndReplacesWithoutLeavingTemporaryFiles)
{
    const auto path = root / "document.json";
    NX::WriteFileAtomically(path, "old contents");
    NX::WriteFileAtomically(path, "new");
    EXPECT_EQ(read(path), "new");
    EXPECT_EQ(std::distance(std::filesystem::directory_iterator(root),
                            std::filesystem::directory_iterator()),
              1);
}

TEST_F(AtomicFileTest, ReplacementFailureKeepsDestinationAndCleansTemporaryFile)
{
    const auto destination = root / "existing";
    std::filesystem::create_directory(destination);
    NX::WriteFileAtomically(destination / "kept", "previous contents");
    EXPECT_THROW(NX::WriteFileAtomically(destination, "new"), std::filesystem::filesystem_error);
    EXPECT_EQ(read(destination / "kept"), "previous contents");
    EXPECT_EQ(std::distance(std::filesystem::directory_iterator(root),
                            std::filesystem::directory_iterator()),
              1);
}

TEST_F(AtomicFileTest, ConcurrentSavesPublishOneCompleteDocument)
{
    const auto path = root / "shared.json";
    const std::string first(16384, 'a');
    const std::string second(16384, 'b');
    std::exception_ptr firstError;
    std::exception_ptr secondError;
    const auto write = [&path](const std::string& text, std::exception_ptr& error)
    {
        try
        {
            NX::WriteFileAtomically(path, text);
        }
        catch (...)
        {
            error = std::current_exception();
        }
    };
    std::thread a(write, std::cref(first), std::ref(firstError));
    std::thread b(write, std::cref(second), std::ref(secondError));
    a.join();
    b.join();
    ASSERT_FALSE(firstError);
    ASSERT_FALSE(secondError);
    const auto saved = read(path);
    EXPECT_TRUE(saved == first || saved == second);
    EXPECT_EQ(std::distance(std::filesystem::directory_iterator(root),
                            std::filesystem::directory_iterator()),
              1);
}

TEST_F(AtomicFileTest, SerializationFailurePreservesPreviouslySavedDocument)
{
    TestDocument document;
    document.directory = root;
    auto& cache = NX::GetCacheSystem();
    cache.write(document, { { "value", 42 } });
    const auto before = read(root / "document.json");
    const auto invalid
        = nlohmann::json{ { "invalidUtf8", std::string(1, static_cast<char>(0xff)) } };
    EXPECT_THROW(cache.write(document, invalid), nlohmann::json::type_error);
    EXPECT_EQ(read(root / "document.json"), before);
    cache.write(document, { { "value", 43 } });
    EXPECT_EQ(nlohmann::json::parse(read(root / "document.json"))["value"], 43);
}

TEST_F(AtomicFileTest, BaseAssetUpdatesItsExistingFile)
{
    const auto path = root / "asset.json";
    NX::WriteFileAtomically(path, "{\"kept\":42}");
    TestAsset asset;
    asset.attachSourceFile(path);
    asset.writeToFile();
    const auto data = nlohmann::json::parse(read(path));
    EXPECT_EQ(data["kept"], 42);
    EXPECT_EQ(data["updated"], true);
}

#ifndef _WIN32
TEST_F(AtomicFileTest, FailedWriteKeepsPreviousFile)
{
    const auto path = root / "document.json";
    NX::WriteFileAtomically(path, "previous contents");
    const std::string largeDocument(16384, 'x');
    const auto child = fork();
    ASSERT_GE(child, 0);
    if (child == 0)
    {
        std::signal(SIGXFSZ, SIG_IGN);
        rlimit limit{};
        if (getrlimit(RLIMIT_FSIZE, &limit) != 0)
        {
            _exit(2);
        }
        limit.rlim_cur = 1024;
        if (setrlimit(RLIMIT_FSIZE, &limit) != 0)
        {
            _exit(2);
        }
        try
        {
            NX::WriteFileAtomically(path, largeDocument);
        }
        catch (const std::filesystem::filesystem_error&)
        {
            _exit(0);
        }
        _exit(1);
    }
    int status = 0;
    ASSERT_EQ(waitpid(child, &status, 0), child);
    ASSERT_TRUE(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), 0);
    EXPECT_EQ(read(path), "previous contents");
    EXPECT_EQ(std::distance(std::filesystem::directory_iterator(root),
                            std::filesystem::directory_iterator()),
              1);
}
#endif

TEST_F(AtomicFileTest, FailedLoadBlocksAutosaveUntilSuccessfulReload)
{
    TestDocument document;
    document.directory = root;
    const auto path = root / "document.json";
    NX::WriteFileAtomically(path, "unreadable document");
    auto& cache = NX::GetCacheSystem();
    cache.read(document);
    cache.write(document, { { "value", 0 } });
    EXPECT_EQ(read(path), "unreadable document");
    NX::WriteFileAtomically(path, "{\"value\":42}");
    cache.read(document);
    EXPECT_EQ(document.value, 42);
    cache.write(document, { { "value", 43 } });
    EXPECT_EQ(nlohmann::json::parse(read(path))["value"], 43);
}
