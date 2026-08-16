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
#include "Grid.h"
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
        using ObjectContainerT = std::vector<SceneObject::Ptr>;

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

        [[nodiscard]] const ObjectContainerT& getObjects() const noexcept { return _sceneObjects; }
        [[nodiscard]] ObjectContainerT& getObjects() noexcept { return _sceneObjects; }

        template<IsComponent T>
        [[nodiscard]] T::Ptr gerFirstOf();

        void addUniqueObjectToScene(SceneObject::Ptr object);
        void addObjectToScene(SceneObject::Ptr object);
        void addBlueprintObjectToScene(const WeakData<ECSAsset>& asset, const StringAtom& name);
        bool deleteFromScene(const StringAtom& name);
        bool deleteFromScene(const BaseComponent* obj);
        bool deleteFromSceneOrFromObject(BaseComponent* obj);
        void duplicateSceneObject(const BaseComponent* obj);
        [[nodiscard]] IntrusivePtr<SceneObject> find(const BaseComponent* obj);

        [[nodiscard]] nlohmann::json serialize() const;
        void deserialize(RResourceStream<RJsonResourceStream>& data);
        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] StringAtom getCacheHash() const override;
        [[nodiscard]] spdlog::logger* getLogger() const override;

        Delegate<void(SceneObject*)>::Ptr onObjectAdded = Delegate<void(SceneObject*)>::Create();

    public:
        Grid grid;
        NXSkybox skybox;

    protected:
        ObjectContainerT _sceneObjects;

        FIELD();
        StringAtom _sceneName = "Default";

    private:
        void internal_addObjectToScene(SceneObject* object);

    private:
        // TODO: get rid of it, deprecated
        std::vector<Actor*> _postDrawBuffer;

        uint32_t _uniqueCounterName = 0;
    };

    template<IsComponent T>
    typename T::Ptr Scene::gerFirstOf()
    {
        for (auto&& obj : _sceneObjects)
        {
            if (auto t = DynamicCast<T>(obj))
            {
                return t;
            }
        }

        return nullptr;
    }

} // namespace Core
#include "Scene.generated.h" // added by the code generator. Better don't move it.
