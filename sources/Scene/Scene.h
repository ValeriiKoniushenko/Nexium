/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "../RawDataManagement/DataStream.h"
#include "AssetsManager/ECSAsset.h"
#include "AssetsManager/Mesh3DAsset.h"
#include "AssetsManager/SkyboxAsset.h"
#include "Core/Delegate.h"
#include "GameplaySystem/Framework/WorldObject.h"
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "Grid.h"

#include <vector>

namespace Core
{
    class Scene : public IDataIO
    {
    public:
        // TODO: change to another data structure!!! It's awful
        using ObjectContainerT = std::vector<WorldObject::Ptr>;

    public:
        Scene();
        ~Scene() override = default;
        Scene(const Scene&) = delete;
        Scene(Scene&&) = delete;

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

        Delegate<void(WorldObject*)>::Ptr onObjectAdded = Delegate<void(WorldObject*)>::Create();

        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] StringAtom getCacheHash() const override;

    public:
        Grid grid;
        NXSkybox skybox;

    protected:
        void writeToCacheSeparateData();

    protected:
        ObjectContainerT _objects;

        StringAtom _sceneName = "Default";

    private:
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