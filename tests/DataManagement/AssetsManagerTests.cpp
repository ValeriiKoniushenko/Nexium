#include "../ECSTests/DummyComponent.h"
#include "AssetsManager/AssetImpls/Factory.h"
#include "AssetsManager/AssetsManager.h"
#include "Misc/Configs.h"

#include "gtest/gtest.h"
#include <chrono>
#include <fstream>

using namespace Core;

namespace
{
    namespace fs = std::filesystem;

    class TestAssetsManager final : public AssetsManager
    {
    public:
        void useOnlyAssetPath(const fs::path& path)
        {
            _registeredPaths.clear();
            registerNewAssetPath(path);
        }

        [[nodiscard]] std::size_t ecsAssetCount() const noexcept { return _ecsAssets.size(); }
        [[nodiscard]] std::size_t textureCount() const noexcept { return _textures.size(); }
        [[nodiscard]] std::size_t skyboxCount() const noexcept { return _skyboxes.size(); }

        [[nodiscard]] ECSAsset* findIndexedEcsAsset(const StringAtom& logicPath)
        {
            const auto it = _ecsAssets.find(logicPath);
            return it == _ecsAssets.end() ? nullptr : it->second.get();
        }

        [[nodiscard]] bool hasIndexedTexture(const StringAtom& logicPath) const
        {
            return _textures.contains(logicPath);
        }

        [[nodiscard]] bool hasIndexedSkybox(const StringAtom& logicPath) const
        {
            return _skyboxes.contains(logicPath);
        }
    };

    class DummyAssetImpl final : public ECSAssetImpl
    {
    public:
        using implementedAssetType = DummyComponent;

        void load(const ECSAsset&, BaseComponent* dataOwner,
                  const nlohmann::json& assetData) override
        {
            ++loadCalls;
            lastDataOwner = dataOwner;
            lastAssetData = assetData;
        }

        void unload(const ECSAsset&, BaseComponent*) override {}

        inline static int loadCalls = 0;
        inline static BaseComponent* lastDataOwner = nullptr;
        inline static nlohmann::json lastAssetData;
    };

    class CountingAsset final : public BaseAsset
    {
    public:
        inline static const char* fileExtension = ".counting";

        explicit CountingAsset(const StringAtom& logicPath)
            : BaseAsset(logicPath)
        {
        }
        CountingAsset(const CountingAsset&) = delete;
        CountingAsset& operator=(const CountingAsset&) = delete;
        CountingAsset(CountingAsset&&) = delete;
        CountingAsset& operator=(CountingAsset&&) = delete;

        ~CountingAsset() override { ++destructions; }

        [[nodiscard]] const char* getPrefix() const override { return "CountingAsset"; }

        static void resetCounters()
        {
            loads = 0;
            unloads = 0;
            destructions = 0;
        }

        inline static int loads = 0;
        inline static int unloads = 0;
        inline static int destructions = 0;

    protected:
        void onLoadRequest() override { ++loads; }
        void onUnloadRequest() override { ++unloads; }
    };

    void RegisterDummyAssetImpl()
    {
        static const bool registered = []
        {
            AssetImpl::GetFactory().registerAssetType<DummyAssetImpl>();
            return true;
        }();
        (void)registered;
    }

    class AssetsManagerTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            const auto uniquePart = std::chrono::steady_clock::now().time_since_epoch().count();
            root = fs::temp_directory_path()
                   / ("nexium_assets_manager_tests_" + std::to_string(uniquePart));
            fs::create_directories(root);
            RegisterDummyAssetImpl();
        }

        void TearDown() override { fs::remove_all(root); }

        [[nodiscard]] StringAtom logicPath(const fs::path& path) const
        {
            return StringAtom::Intern(
                fs::relative(fs::absolute(path), Config::Path::projectAbsPath).generic_string());
        }

        void writeEcsAsset(const fs::path& path, std::string_view name,
                           std::string_view tags = "WorldObject")
        {
            const DummyComponent component;
            nlohmann::json json = {
                { ECSAsset::StreamData::type, DummyComponent::componentType },
                { ECSAsset::StreamData::name, name },
                { ECSAsset::StreamData::tags, tags },
                { ECSAsset::StreamData::data, component.serialize() },
                { ECSAsset::StreamData::assetData, { { "marker", 42 } } },
            };
            writeJson(path, json);
        }

        void writeTextureAsset(const fs::path& path)
        {
            writeJson(path, { { "AssetData",
                                { { "_path", "missing-test-image.png" },
                                  { "_isFlipVertically", false } } } });
        }

        void writeSkyboxAsset(const fs::path& path)
        {
            writeJson(path, { { "AssetData",
                                { { "_paths", std::array<std::string, 6>{} },
                                  { "_isFlipVertically", false } } } });
        }

        void writeTextFile(const fs::path& path, std::string_view contents = {})
        {
            fs::create_directories(path.parent_path());
            std::ofstream out(path);
            ASSERT_TRUE(out.is_open());
            out << contents;
        }

        void writeJson(const fs::path& path, const nlohmann::json& json)
        {
            writeTextFile(path, json.dump(2));
        }

        fs::path root;
    };
} // namespace

TEST_F(AssetsManagerTest, ConstructorRegistersDataPathAndCreatesDefaultAtlas)
{
    const TestAssetsManager manager;

    ASSERT_TRUE(manager.getRegisteredPaths().contains(Config::Path::data));
    ASSERT_EQ(1u, manager.getAtlasesCount());
    ASSERT_EQ(&manager.getTextureAtlas(), &manager.getAtlas("default"_atom));
    ASSERT_EQ((std::vector<StringAtom>{ "default"_atom }), manager.getAtlasesAsVector());
    ASSERT_THROW((void)manager.getAtlas("missing"_atom), std::runtime_error);

    manager.registerNewAssetPath(root);
    manager.registerNewAssetPath(root);
    ASSERT_EQ(2u, manager.getRegisteredPaths().size());
}

TEST_F(AssetsManagerTest, NodeTypeRecognizesFoldersCodeImagesAndNxAssets)
{
    const auto folder = root / "folder";
    fs::create_directory(folder);
    writeTextFile(root / "source.cpp");
    writeTextFile(root / "image.png");
    writeTextFile(root / "asset.nxtex");
    writeTextFile(root / "document.bin");

    ASSERT_EQ(AssetsManager::NodeType::Folder,
              AssetsManager::GetNodeType(fs::directory_entry(folder)));
    ASSERT_EQ(AssetsManager::NodeType::Code,
              AssetsManager::GetNodeType(fs::directory_entry(root / "source.cpp")));
    ASSERT_EQ(AssetsManager::NodeType::Image,
              AssetsManager::GetNodeType(fs::directory_entry(root / "image.png")));
    ASSERT_EQ(AssetsManager::NodeType::NxFile,
              AssetsManager::GetNodeType(fs::directory_entry(root / "asset.nxtex")));
    ASSERT_EQ(AssetsManager::NodeType::Default,
              AssetsManager::GetNodeType(fs::directory_entry(root / "document.bin")));
}

TEST_F(AssetsManagerTest, InitialScanIndexesEverySupportedAssetType)
{
    const auto ecsPath = root / "actor.nx";
    const auto texturePath = root / "texture.nxtex";
    const auto skyboxPath = root / "sky.nxsky";
    writeEcsAsset(ecsPath, "Actor", "WorldObject,GuiObject");
    writeTextureAsset(texturePath);
    writeSkyboxAsset(skyboxPath);
    writeTextFile(root / "ignored.nxunknown");

    TestAssetsManager manager;
    manager.useOnlyAssetPath(root);
    manager.initScanFileSystem();

    ASSERT_EQ(1u, manager.ecsAssetCount());
    ASSERT_EQ(1u, manager.textureCount());
    ASSERT_EQ(1u, manager.skyboxCount());

    const auto ecsId = logicPath(ecsPath);
    const auto* ecsAsset = manager.findIndexedEcsAsset(ecsId);
    ASSERT_NE(nullptr, ecsAsset);
    ASSERT_EQ(ECSAsset::Status::PreLoaded, ecsAsset->getLoadingStatus());
    ASSERT_EQ("Actor", ecsAsset->getName());
    ASSERT_EQ(Tag_WorldObject | Tag_GuiObject, ecsAsset->getTags());

    ASSERT_TRUE(manager.hasIndexedTexture(logicPath(texturePath)));
    ASSERT_TRUE(manager.hasIndexedSkybox(logicPath(skyboxPath)));
}

TEST_F(AssetsManagerTest, InitialScanIsIndependentForEachManagerInstance)
{
    writeEcsAsset(root / "actor.nx", "Actor");

    TestAssetsManager first;
    first.useOnlyAssetPath(root);
    first.initScanFileSystem();

    TestAssetsManager second;
    second.useOnlyAssetPath(root);
    second.initScanFileSystem();

    ASSERT_EQ(1u, first.ecsAssetCount());
    ASSERT_EQ(1u, second.ecsAssetCount());
}

TEST_F(AssetsManagerTest, RefreshAddsNewAssetsAndRemovesDeletedAssets)
{
    const auto removedPath = root / "removed.nx";
    const auto texturePath = root / "new.nxtex";
    const auto skyboxPath = root / "new.nxsky";
    writeEcsAsset(removedPath, "Removed");

    TestAssetsManager manager;
    manager.useOnlyAssetPath(root);
    manager.initScanFileSystem();
    ASSERT_EQ(1u, manager.ecsAssetCount());

    fs::remove(removedPath);
    writeTextureAsset(texturePath);
    writeSkyboxAsset(skyboxPath);
    manager.refreshFilesSystem();

    ASSERT_EQ(0u, manager.ecsAssetCount());
    ASSERT_EQ(1u, manager.textureCount());
    ASSERT_EQ(1u, manager.skyboxCount());
    ASSERT_TRUE(manager.hasIndexedTexture(logicPath(texturePath)));
    ASSERT_TRUE(manager.hasIndexedSkybox(logicPath(skyboxPath)));
}

TEST_F(AssetsManagerTest, RefreshUpdatesPreloadedEcsMetadata)
{
    const auto path = root / "actor.nx";
    writeEcsAsset(path, "Before", "WorldObject");

    TestAssetsManager manager;
    manager.useOnlyAssetPath(root);
    manager.initScanFileSystem();
    auto strong = manager.getEcsAsset(logicPath(path));
    ASSERT_EQ(ECSAsset::Status::Loaded, strong->getLoadingStatus());

    writeEcsAsset(path, "After", "GuiObject");
    manager.refreshFilesSystem();

    const auto* asset = manager.findIndexedEcsAsset(logicPath(path));
    ASSERT_NE(nullptr, asset);
    ASSERT_EQ("After", asset->getName());
    ASSERT_EQ(Tag_GuiObject, asset->getTags());
    ASSERT_EQ(ECSAsset::Status::Loaded, asset->getLoadingStatus());
}

TEST_F(AssetsManagerTest, EcsLookupHandlesMissingAssetsAndPreservesWeakLoadingSemantics)
{
    const auto path = root / "actor.nx";
    writeEcsAsset(path, "Actor");

    TestAssetsManager manager;
    manager.useOnlyAssetPath(root);
    manager.initScanFileSystem();
    const auto id = logicPath(path);

    auto weak = manager.getWeakEcsAsset(id);
    ASSERT_TRUE(weak.isValid());
    ASSERT_EQ(ECSAsset::Status::PreLoaded, manager.findIndexedEcsAsset(id)->getLoadingStatus());

    auto strong = manager.getEcsAsset(id);
    ASSERT_TRUE(strong.isValid());
    ASSERT_EQ(ECSAsset::Status::Loaded, strong->getLoadingStatus());
    ASSERT_TRUE(strong->getData().isValid());
    strong.reset();
    ASSERT_EQ(ECSAsset::Status::PreLoaded, manager.findIndexedEcsAsset(id)->getLoadingStatus());

    ASSERT_FALSE(manager.getEcsAsset("missing.nx"_atom).isValid());
    ASSERT_FALSE(manager.getWeakEcsAsset("missing.nx"_atom).isValid());
    ASSERT_FALSE(manager.getTexture("missing.nxtex"_atom).isValid());
    ASSERT_FALSE(manager.getSkybox("missing.nxsky"_atom).isValid());
}

TEST_F(AssetsManagerTest, FilesystemPathLookupUsesTheProjectRelativeAssetId)
{
    const auto path = root / "nested" / "actor.nx";
    writeEcsAsset(path, "Actor");

    TestAssetsManager manager;
    manager.useOnlyAssetPath(root);
    manager.initScanFileSystem();

    const auto weak = manager.getWeakEcsAssetByPath(fs::absolute(path));
    ASSERT_TRUE(weak.isValid());
    ASSERT_EQ(logicPath(path), weak.tryLoad()->getLogicPath());
    ASSERT_FALSE(manager.getWeakEcsAssetByPath(root / "wrong.nx").isValid());
    ASSERT_FALSE(manager.getWeakEcsAssetByPath(root / "actor.txt").isValid());
}

TEST_F(AssetsManagerTest, TagFilteredCountAndIndexedAccessUseTheSameSelection)
{
    writeEcsAsset(root / "world.nx", "World", "WorldObject");
    writeEcsAsset(root / "gui.nx", "Gui", "GuiObject");
    writeEcsAsset(root / "both.nx", "Both", "WorldObject,GuiObject");

    TestAssetsManager manager;
    manager.useOnlyAssetPath(root);
    manager.initScanFileSystem();

    ASSERT_EQ(3u, manager.getEcsAssetCountByTag(Tag_Any));
    ASSERT_EQ(2u, manager.getEcsAssetCountByTag(Tag_WorldObject));
    ASSERT_EQ(2u, manager.getEcsAssetCountByTag(Tag_GuiObject));
    ASSERT_EQ(0u, manager.getEcsAssetCountByTag(Tag_None));

    for (std::size_t i = 0; i < manager.getEcsAssetCountByTag(Tag_WorldObject); ++i)
    {
        const auto strong = manager.getEcsAssetAt(i, Tag_WorldObject);
        const auto weak = manager.getWeakEcsAssetAt(i, Tag_WorldObject);
        ASSERT_TRUE(strong.isValid());
        ASSERT_TRUE(weak.isValid());
        ASSERT_TRUE(static_cast<bool>(strong->getTags() & Tag_WorldObject));
        ASSERT_EQ(strong.get(), weak.tryLoad().get());
    }

    ASSERT_FALSE(manager.getEcsAssetAt(2, Tag_WorldObject).isValid());
    ASSERT_FALSE(manager.getWeakEcsAssetAt(2, Tag_WorldObject).isValid());
}

TEST_F(AssetsManagerTest, UniqueEcsLoadPassesTheNewComponentToItsAssetAdapter)
{
    const auto path = root / "actor.nx";
    writeEcsAsset(path, "Actor");

    TestAssetsManager manager;
    manager.useOnlyAssetPath(root);
    manager.initScanFileSystem();
    DummyAssetImpl::loadCalls = 0;
    DummyAssetImpl::lastDataOwner = nullptr;
    DummyAssetImpl::lastAssetData.clear();

    const auto unique = manager.getUniqueEcsAsset(logicPath(path));

    ASSERT_TRUE(unique.isValid());
    ASSERT_EQ(1, DummyAssetImpl::loadCalls);
    ASSERT_EQ(unique.get(), DummyAssetImpl::lastDataOwner);
    ASSERT_EQ(42, DummyAssetImpl::lastAssetData.at("marker"));
    ASSERT_EQ(ECSAsset::Status::PreLoaded,
              manager.findIndexedEcsAsset(logicPath(path))->getLoadingStatus());
    ASSERT_FALSE(manager.getUniqueEcsAsset("missing.nx"_atom).isValid());
}

TEST_F(AssetsManagerTest, SyncEcsAssetPreservesAdapterSpecificData)
{
    const auto path = root / "actor.nx";
    writeEcsAsset(path, "Actor");

    TestAssetsManager manager;
    manager.useOnlyAssetPath(root);
    manager.initScanFileSystem();
    auto asset = manager.getEcsAsset(logicPath(path));
    ASSERT_TRUE(asset.isValid());

    asset->syncAssetWithMemory(asset->getData()->serialize());

    std::ifstream input(path);
    ASSERT_TRUE(input.is_open());
    const auto persisted = nlohmann::json::parse(input);
    ASSERT_EQ(42, persisted.at(ECSAsset::StreamData::assetData).at("marker"));
}

TEST_F(AssetsManagerTest, UnloadAllResourcesInvalidatesIndexedWeakAssets)
{
    const auto path = root / "actor.nx";
    writeEcsAsset(path, "Actor");

    TestAssetsManager manager;
    manager.useOnlyAssetPath(root);
    manager.initScanFileSystem();
    auto weak = manager.getWeakEcsAsset(logicPath(path));
    ASSERT_TRUE(weak.isValid());

    manager.unloadAllResources();

    ASSERT_EQ(0u, manager.ecsAssetCount());
    ASSERT_EQ(0u, manager.textureCount());
    ASSERT_EQ(0u, manager.skyboxCount());
    ASSERT_FALSE(weak.tryLoad().isValid());
}

TEST(AssetRefTest, FirstConsumerLoadsAndLastConsumerUnloadsTheIndexedAsset)
{
    CountingAsset::resetCounters();
    {
        AssetRef<CountingAsset> indexed(new CountingAsset("asset.counting"_atom));
        ASSERT_FALSE(indexed->isLoaded());
        {
            AssetRef<CountingAsset> firstConsumer(indexed);
            ASSERT_TRUE(indexed->isLoaded());
            ASSERT_EQ(1, CountingAsset::loads);
            {
                AssetRef<CountingAsset> secondConsumer(firstConsumer);
                ASSERT_EQ(1, CountingAsset::loads);
            }
            ASSERT_EQ(0, CountingAsset::unloads);
        }
        ASSERT_FALSE(indexed->isLoaded());
        ASSERT_EQ(1, CountingAsset::unloads);
        ASSERT_EQ(0, CountingAsset::destructions);
    }
    ASSERT_EQ(1, CountingAsset::destructions);
}

TEST(AssetRefTest, MoveAssignmentTransfersOwnershipWithoutAnExtraDecrement)
{
    CountingAsset::resetCounters();
    {
        AssetRef<CountingAsset> indexed(new CountingAsset("asset.counting"_atom));
        AssetRef<CountingAsset> consumer(indexed);
        AssetRef<CountingAsset> destination;

        destination = std::move(consumer);

        ASSERT_FALSE(consumer.isValid());
        ASSERT_TRUE(destination.isValid());
        ASSERT_TRUE(indexed->isLoaded());
    }
    ASSERT_EQ(1, CountingAsset::loads);
    ASSERT_EQ(1, CountingAsset::unloads);
    ASSERT_EQ(1, CountingAsset::destructions);
}
