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
        void addToScene(T&& object, bool readFromCache = false)
        {
            _objects.emplace_back(new T(std::forward<decltype(object)>(object)));
            auto* added = _objects.back().get();

            added->initialize();
            onObjectAdded->trigger(added);
            if (readFromCache)
            {
                added->tryReadFromCache();
            }
        }

        template<IsWorldObjectBased T, class... Args>
        T* createAndGet(Args... args)
        {
            _objects.emplace_back(new T(std::forward<Args>(args)...));
            auto* added = _objects.back().get();

            added->initialize();
            onObjectAdded->trigger(added);
            return reinterpret_cast<T*>(added);
        }

        [[nodiscard]] const std::vector<WorldObject::Ptr>& getObjects() const noexcept
        {
            return _objects;
        }
        [[nodiscard]] std::vector<WorldObject::Ptr>& getObjects() noexcept { return _objects; }

        template<IsWorldObjectBased T>
        [[nodiscard]] T* gerFirstOf()
        {
            auto it = std::ranges::find_if(_objects, [](const WorldObject::Ptr& actor)
                                           { return actor->isTypeOf<T>(); });

            return it == _objects.end() ? nullptr : reinterpret_cast<T*>(it->get());
        }

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
} // namespace Core