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

#include "AssetsManager/ECSAsset.h"
#include "AssetsManager/Mesh3DAsset.h"
#include "AssetsManager/SkyboxAsset.h"
#include "Core/Delegate.h"
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "Grid.h"
#include "Misc/DataStream.h"

#include <vector>

namespace Core
{
    class Scene : public IDataStreamBridge
    {
    public:
        Scene();
        ~Scene() override = default;
        Scene(const Scene&) = delete;
        Scene(Scene&&) = delete;

        void initialize();

        void tick(float timeDelta);

        void directDraw();

        void setSceneName(StringAtom name);

        void ioFieldsUpdate(DataStream& out) override;

        [[nodiscard]] const StringAtom& getSceneName() const noexcept;

        template<IsWorldObjectBased T>
        void addToScene(const T& object, bool readFromCache = false);

        template<IsWorldObjectBased T, class... Args>
        T::Ptr createAndGet(Args... args);

        [[nodiscard]] const std::vector<WorldObject::Ptr>& getObjects() const noexcept
        {
            return _objects;
        }
        [[nodiscard]] std::vector<WorldObject::Ptr>& getObjects() noexcept { return _objects; }

        template<IsWorldObjectBased T>
        [[nodiscard]] T::Ptr gerFirstOf();

        Delegate<void(WorldObject*)>::Ptr onObjectAdded = Delegate<void(WorldObject*)>::Create();

    public:
        Grid grid;
        NXSkybox skybox;

    protected:
        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] StringAtom getCacheHash() const override;

        void writeToCacheSeparateData();

    protected:
        // TODO: change to another data structure!!! It's awful
        std::vector<WorldObject::Ptr> _objects;

        StringAtom _sceneName = "Default";

    private:
        std::vector<Actor*> _postDrawBuffer;
    };

    template<IsWorldObjectBased T>
    void Scene::addToScene(const T& object, bool readFromCache)
    {
        _objects.emplace_back(StaticCast<T>(object.clone()));
        auto* added = _objects.back().get();

        added->initialize();
        if (readFromCache)
        {
            added->tryReadFromCache();
        }

        onObjectAdded->trigger(added);
    }

    template<IsWorldObjectBased T, class... Args>
    typename T::Ptr Scene::createAndGet(Args... args)
    {
        _objects.emplace_back(new T(std::forward<Args>(args)...));
        auto added = _objects.back();

        added->initialize();
        onObjectAdded->trigger(added.get());
        return StaticCast<T>(added);
    }

    template<IsWorldObjectBased T>
    typename T::Ptr Scene::gerFirstOf()
    {
        for (auto&& obj : _objects)
        {
            if (auto* t = dynamic_cast<T*>(obj.get()))
            {
                return StaticCast<T>(obj);
            }
        }

        return nullptr;
    }

} // namespace Core