/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "AssetsManager/ECSAsset.h"
#include "AssetsManager/SkyboxAsset.h"
#include "Core/Delegate.h"
#include "GameplaySystem/Framework/WorldObject.h"
#include "Grid.h"
#include "ResourceManagement/DataStream.h"
#include "SceneObject.h"

#include <vector>

namespace Core
{
    class Actor;

    CLASS();
    class Scene : public IDataIO, public BaseLog
    {
        R_FRIEND(Scene);

    public:
        // TODO: change to another data structure!!! It's awful
        using ObjectContainerT = std::vector<WorldObject::Ptr>;

        struct StreamData
        {
            StreamData() = delete;

            static constexpr const char* sceneObjects = "sceneObjects";
        };

    public:
        Scene() = default;
        ~Scene() override = default;
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        void initialize();

        void tick(float timeDelta);

        void directDraw();

        void setSceneName(StringAtom name);

        [[nodiscard]] const StringAtom& getSceneName() const noexcept;

        void addToScene(const BaseComponent* object, bool readFromCache = false);
        void addToScene(ECSAsset& asset, bool readFromCache = false);

        template<IsComponent T, class... Args>
        [[nodiscard]] typename T::Ptr createAndGet(Args... args);

        [[nodiscard]] const ObjectContainerT& getObjects() const noexcept { return _objects; }
        [[nodiscard]] ObjectContainerT& getObjects() noexcept { return _objects; }

        template<IsComponent T>
        [[nodiscard]] T::Ptr gerFirstOf();

        void addBlueprintObjectToScene(const WeakData<ECSAsset>& asset);

        [[nodiscard]] nlohmann::json serialize() const;
        void deserialize(RResourceStream<RJsonResourceStream>& data);
        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] StringAtom getCacheHash() const override;
        [[nodiscard]] spdlog::logger* getLogger() const override;

        Delegate<void(WorldObject*)>::Ptr onObjectAdded = Delegate<void(WorldObject*)>::Create();

    public:
        Grid grid;
        NXSkybox skybox;
        std::vector<SceneObject::Ptr> _sceneObjects;

    protected:
        void writeToCacheSeparateData();

    protected:
        // TODO: get rid of it, deprecated
        ObjectContainerT _objects;

        FIELD();
        StringAtom _sceneName = "Default";

    private:
        // TODO: get rid of it, deprecated
        std::vector<Actor*> _postDrawBuffer;
    };

    template<IsComponent T, class... Args>
    typename T::Ptr Scene::createAndGet(Args... args)
    {
        _objects.emplace_back(new WorldObject);
        auto* sceneAsset = _objects.back().get();

        if (!sceneAsset->spawnData<T>(std::forward<Args>(args)...))
        {
            return nullptr;
        }

        auto* added = sceneAsset->getData();

        added->initialize();
        onObjectAdded->trigger(sceneAsset);
        return static_cast<T*>(added);
    }

    template<IsComponent T>
    typename T::Ptr Scene::gerFirstOf()
    {
        for (auto&& asset : _objects)
        {
            if (auto* obj = asset->getData())
            {
                if (auto* t = dynamic_cast<T*>(obj))
                {
                    return typename T::Ptr(static_cast<T*>(obj));
                }
            }
        }

        return nullptr;
    }

} // namespace Core
#include "Scene.generated.h" // added by the code generator. Better don't move it.
