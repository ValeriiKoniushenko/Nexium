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
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "InputDevices/ModuleInfo.h"

#include <vector>

namespace SW
{
    class Scene : public JsonCacheable, public JsonAdapter
    {
    public:
        Scene() = default;
        ~Scene() override = default;

        void tick(float timeDelta);

        void directDraw();

        void setSceneName(Core::StringAtom name);
        [[nodiscard]] const Core::StringAtom& getSceneName() const noexcept;

        void addMesh(StaticMeshBundle&& mesh);

        [[nodiscard]] const std::vector<StaticMeshBundle>& getStaticMeshBundles() const noexcept
        {
            return _staticMeshBundles;
        }
        [[nodiscard]] std::vector<StaticMeshBundle>& getStaticMeshBundles() noexcept
        {
            return _staticMeshBundles;
        }
        nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren) override;

        /**
         * @brief You can use it to add some logical stuff. I.e.:
         * Spectator, some TriggerBoxes - in general everything
         * that shouldn't be rendered.
         */
        template<IsComponent CompT, class... Args>
        CompT* createAndGetLogicalComponent(Args&&... args)
        {
            typename CompT::Ptr newComp = new CompT(std::forward<Args>(args)...);
            newComp->initialize();
            _logicalComponents.push_back(std::move(newComp));
            return static_cast<CompT*>(_logicalComponents.back().get());
        }

    protected:
        std::filesystem::path getCacheDir() const override;
        Core::StringAtom getCacheHash() const override;
        nlohmann::json toCacheData() const override;
        void fromCacheData(const nlohmann::json& json) override;

    protected:
        void forceWriteToCacheAllMeshes() const;

    protected:
        std::vector<StaticMeshBundle> _staticMeshBundles;
        std::vector<BaseComponent::Ptr> _logicalComponents;
        Core::StringAtom _sceneName = "None";
    };
} // namespace SW