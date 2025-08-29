// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include "Editor/Gizmo.h"
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "Grid.h"
#include "InputDevices/ModuleInfo.h"

#include <vector>

namespace Core
{
    class Scene : public JsonCacheable, public JsonAdapter
    {
    public:
        Scene();
        ~Scene() override = default;

        void tick(float timeDelta);

        void directDraw();

        void setSceneName(StringAtom name);
        [[nodiscard]] const StringAtom& getSceneName() const noexcept;

        template<IsActorBased T>
        void addActor(T&& actor, bool readFromCache = false)
        {
            _actors.emplace_back(new T(std::forward<decltype(actor)>(actor)));
            auto* added = _actors.back().get();

            added->initialize();
            onActorAdded.trigger(added);
            if (readFromCache)
            {
                added->tryReadFromCache();
            }
        }

        template<IsActorBased T, class... Args>
        T* createAndGetActor(Args... args)
        {
            _actors.emplace_back(new T(std::forward<Args>(args)...));
            auto* added = _actors.back().get();

            added->initialize();
            onActorAdded.trigger(added);
            return reinterpret_cast<T*>(added);
        }

        [[nodiscard]] const std::vector<Actor::Ptr>& getActors() const noexcept { return _actors; }
        [[nodiscard]] std::vector<Actor::Ptr>& getActors() noexcept { return _actors; }

        template<IsActorBased T>
        [[nodiscard]] T* getFirstActorOf()
        {
            auto it = std::ranges::find_if(_actors,
                                           [](const Actor::Ptr& actor)
                                           {
                                               return actor->isTypeOf<T>();
                                           });

            return it == _actors.end() ? nullptr : reinterpret_cast<T*>(it->get());
        }

        [[nodiscard]] nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren) override;

        Delegate<void(Actor*)> onActorAdded;

    public:
        Grid grid;

    protected:
        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] StringAtom getCacheHash() const override;
        [[nodiscard]] nlohmann::json toCacheData() const override;
        void fromCacheData(const nlohmann::json& json) override;
        void writeToCacheSeparateData() const;

    protected:
        std::vector<Actor::Ptr> _actors;
        StringAtom _sceneName = "None";
        Gizmo* _gizmo = nullptr;
    };
} // namespace Core