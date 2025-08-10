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
#include "Core/String.h"
#include "GameplaySystem/ECS/BaseComponent.h"
#include "GameplaySystem/Framework/Actor.h"
#include "ImGui/imgui.h"
#include "Misc/ImGuiHelper.h"
#include "Scene/Scene.h"

#include <Core/Delegate.h>
#include <Core/Timer.h>

namespace SW
{
    class Scene;
    class BaseCamera;

    /**
     * @brief BaseEditorWindowComponent or briefly BaseEWC
     */
    class BaseEWC : public BaseComponent
    {
        ECS_REGISTER_NEW_COMPONENT(BaseEWC, BaseComponent);

    public:
        [[nodiscard]] const Core::StringAtom& getWindowTitle() { return getComponentName(); }

    protected:
        void onTick() final;
        virtual void onUpdate() {};
        virtual void onDraw() = 0;

        [[nodiscard]] virtual bool beginWindowDraw() = 0;
        virtual void endWindowDraw() = 0;

        [[nodiscard]] bool canBeShown() const { return _isEnabled; }

    protected:
        ImGuiWindowFlags _windowFlags = 0;
    };

    template<class T>
    concept IsEditorWindowComponent = std::derived_from<T, BaseEWC> && IsComponent<T>;

    template<class T>
    concept IsEditorWindowComponentOrVoid = IsEditorWindowComponent<T> || std::is_void_v<T>;

    template<class T>
    concept IsEditorWindowComponentOrBase = IsEditorWindowComponent<T> || std::same_as<T, BaseEWC>;

    class BaseFloatEWC : public BaseEWC
    {
        ECS_REGISTER_NEW_COMPONENT(BaseFloatEWC, BaseEWC);

    public:
        [[nodiscard]] Core::FSize2 getWindowSize() const noexcept { return _size; }
        [[nodiscard]] Core::FSize2 getInnerWindowSize() const noexcept { return _innerSize; }

        void setFitContent(bool v);
        [[nodiscard]] bool isFitContent() const noexcept { return _isFitContent; }

        /**
         * @brief will be called while the window's size changing
         * @param Core::FSize2 new outer(full) size
         * @param Core::FSize2 new inner size
         */
        Core::Delegate<void(Core::FSize2, Core::FSize2)> onSizeChanged;

    protected:
        void onInit() override;
        void onUpdate() override;
        [[nodiscard]] bool beginWindowDraw() override;
        void endWindowDraw() override;

    protected:
        Core::FSize2 _size;
        Core::FSize2 _innerSize;
        Core::FSize2 _oldSize = Core::FSize2{ -1, -1 };
        bool _isFitContent = false;
        std::unordered_map<ImGuiStyleVar_, float> _styles;
    };

    class BaseMenuBarEWC : public BaseEWC
    {
        ECS_REGISTER_NEW_COMPONENT(BaseMenuBarEWC, BaseEWC);

    public:
        void onInit() override;

    protected:
        [[nodiscard]] bool beginWindowDraw() override;
        void endWindowDraw() override;
    };

    class GameViewportEWC : public BaseFloatEWC
    {
        ECS_REGISTER_NEW_COMPONENT(GameViewportEWC, BaseFloatEWC);

    public:
    protected:
        void onInit() override;
        void onDraw() override;
    };

    class KeyboardShortcutsEWC : public BaseFloatEWC
    {
        ECS_REGISTER_NEW_COMPONENT(KeyboardShortcutsEWC, BaseFloatEWC);

    public:
    private:
        void onInit() override;
        void onDraw() override;
    };

    class RootDockWindow : public BaseEWC
    {
        ECS_REGISTER_NEW_COMPONENT(RootDockWindow, BaseEWC);

    public:
    private:
        void onInit() override;
        void onDraw() override;

    protected:
        [[nodiscard]] bool beginWindowDraw() override;
        void endWindowDraw() override;
    };

    class LogsWindow : public BaseFloatEWC, public JsonCacheable
    {
        ECS_REGISTER_NEW_COMPONENT(LogsWindow, BaseFloatEWC);

    public:
        struct LogLine
        {
            Core::StringAtom message;
            spdlog::level::level_enum level = {};
        };

    public:
        ~LogsWindow() override;
        void addLog(Core::StringAtom&& log, spdlog::level::level_enum level);
        void clearLogs();

    protected:
        std::filesystem::path getCacheDir() const override;
        Core::StringAtom getCacheHash() const override;
        nlohmann::json toCacheData() const override;
        void fromCacheData(const nlohmann::json& json) override;

    protected:
        void onInit() override;
        void onDraw() override;
        void onUpdate() override;

    private:
        void detectManualScroll();
        void fetchLogs();
        void toolbarDraw();
        void logsDraw();

    private:
        inline static const std::vector<spdlog::level::level_enum> _levels = {
            spdlog::level::level_enum::critical, spdlog::level::level_enum::err,
            spdlog::level::level_enum::warn,     spdlog::level::level_enum::info,
            spdlog::level::level_enum::debug,    spdlog::level::level_enum::trace,
        };
        std::unordered_map<spdlog::level::level_enum, bool> _levelFilter = {
            { spdlog::level::level_enum::critical, true },
            { spdlog::level::level_enum::err, true },
            { spdlog::level::level_enum::warn, true },
            { spdlog::level::level_enum::info, true },
            { spdlog::level::level_enum::debug, true },
            { spdlog::level::level_enum::trace, true },
        };

        std::size_t _logLimit = 500.f;
        float _defaultGap = 4.f;
        float _clearButtonWidth = 0.f;
        float _autoScrollButtonWidth = 0.f;
        bool _isAutoScroll = true;
        float _streamingToolbarHeight = 40.f;
        float _toolbarToolsWidth = 150.f;
        Core::StringAtom _filterBuf;
        std::size_t _lastCountOfLogs = 0;
        std::list<LogLine> _logs;
    };

    class ObjectPropertiesWindow : public BaseFloatEWC
    {
        ECS_REGISTER_NEW_COMPONENT(ObjectPropertiesWindow, BaseFloatEWC);

    public:
        void setTargetObject(AbstractComponent* actor);
        void resetTargetObject();

    protected:
        void onInit() override;
        void onDraw() override;
        void onUpdate() override;

        void tryDrawTransformable(Transformable* comp);
        void tryDrawBaseComponent(BaseComponent* comp);
        void tryDrawStaticMeshBundle(StaticMeshBundle* comp);
        void tryDrawBaseComponentExtra(BaseComponent* comp);
        void tryDrawGraphicsComponentData(GraphicsComponentData* comp);
        void tryDrawBaseCamera(BaseCamera* comp);

    private:
        AbstractComponent* _target = nullptr;
        Core::Repeater _slowUpdater;
        std::vector<std::pair<int, int>> _graphicsMods;

        // For transform
        Vec3Control _transformLocationControl;
        Vec3Control _transformOriginControl;
        Vec3Control _transformScaleControl;
        Vec3Control _transformRotationControl;

        // For mesh
        Vec3Control _meshSizeControl;

        // For camera
        Vec2Control _frameSizeControl;

        // For Graphics
        std::vector<Core::StringAtom> _modifierValueVec;
        std::vector<Core::StringAtom> _modifierVec;
        std::vector<char> _modifierValueRaw;
        std::vector<char> _modifierRaw;

        // Base settings
        float _labelWidth = 90.f;
        const ImVec2 _overriddenSpacing = ImVec2(0, 6);
        const float _gapBetweenSections = 15.f;

    private:
        [[nodiscard]] int getIndexFromModifier(GraphicsComponentData::ModifiedValue v) const;
        [[nodiscard]] int getIndexFromModifier(GraphicsComponentData::Modifier v) const;
    };

    class SceneTreeWindow : public BaseFloatEWC
    {
        ECS_REGISTER_NEW_COMPONENT(SceneTreeWindow, BaseFloatEWC);

    public:
        void setScene(Scene* scene) { _scene = scene; }
        [[nodiscard]] Scene* getScene() const noexcept { return _scene; }

    public:
        BaseComponent* selectedObject = nullptr;

    protected:
        void onInit() override;
        void onDraw() override;
        void onUpdate() override;

    protected:
        Scene* _scene = nullptr;
        int _commonTreeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick;

    private:
        void drawTreeNode(BaseComponent* n, int32_t id, bool isInSelectedSubtree = false);
    };

} // namespace SW