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

#include "EditorWindow.h"

#include "Camera/Camera.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Misc/IconsFontAwesome.h"
#include "Misc/ImGuiHelper.h"

namespace
{
    static constexpr ImVec4 ColorRed = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
    static constexpr ImVec4 ColorGreen = ImVec4(0.1f, 1.0f, 0.1f, 1.0f);
    static constexpr ImVec4 ColorGrey = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
    static constexpr ImVec4 ColorYellow = ImVec4(1.f, 1.0f, 0.1f, 1.0f);
    static constexpr ImVec4 ColorHalfYellow = ImVec4(0.8f, 0.4f, 0.1f, 1.0f);
    static constexpr ImVec4 ColorBlue = ImVec4(0.1f, 0.1f, 1.0f, 1.0f);

} // namespace

namespace SW
{

    ECS_REGISTER_NEW_COMPONENT_TYPE(BaseEWC)
    ECS_REGISTER_NEW_COMPONENT_TYPE(BaseFloatEWC)
    ECS_REGISTER_NEW_COMPONENT_TYPE(BaseMenuBarEWC)
    ECS_REGISTER_NEW_COMPONENT_TYPE(KeyboardShortcutsEWC)
    ECS_REGISTER_NEW_COMPONENT_TYPE(RootDockWindow)
    ECS_REGISTER_NEW_COMPONENT_TYPE(LogsWindow)
    ECS_REGISTER_NEW_COMPONENT_TYPE(ObjectPropertiesWindow)
    ECS_REGISTER_NEW_COMPONENT_TYPE(SceneTreeWindow)

    // ========================================================================
    //
    //    ______
    //    | ___ \
    //    | |_/ /  __ _  ___   ___
    //    | ___ \ / _` |/ __| / _ \
    //    | |_/ /| (_| |\__ \|  __/
    //    \____/  \__,_||___/ \___|
    // ========================================================================

    void BaseEWC::onTick()
    {
        if (beginWindowDraw())
        {
            onUpdate(); // in the future maybe will be called not every tick
            onDraw();
        }
        endWindowDraw();
    }

    // ========================================================================
    //
    //    ______                   ______  _                _
    //    | ___ \                  |  ___|| |              | |
    //    | |_/ /  __ _  ___   ___ | |_   | |  ___    __ _ | |_
    //    | ___ \ / _` |/ __| / _ \|  _|  | | / _ \  / _` || __|
    //    | |_/ /| (_| |\__ \|  __/| |    | || (_) || (_| || |_
    //    \____/  \__,_||___/ \___|\_|    |_| \___/  \__,_| \__|
    // ========================================================================
    void BaseFloatEWC::setFitContent(bool v)
    {
        _isFitContent = v;
        if (v)
        {
            _windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
        }
        else
        {
            _windowFlags &= ~ImGuiWindowFlags_AlwaysAutoResize;
        }
    }

    void BaseFloatEWC::onUpdate()
    {
        const ImVec2 size = ImGui::GetWindowSize();
        _size = Core::FSize2{ size.x, size.y };

        if (_size != _oldSize)
        {
            const auto regionMax = ImGui::GetContentRegionMax();
            const auto regionMin = ImGui::GetWindowContentRegionMin();
            _innerSize = Core::FSize2(regionMax.x - regionMin.x, regionMax.y - regionMin.y);
            onSizeChanged.trigger(_size, _innerSize);
            _oldSize = _size;
        }
    }

    bool BaseFloatEWC::beginWindowDraw()
    {
        if (_isFitContent)
        {
            ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        }

        for (auto [style, val] : _styles)
        {
            ImGui::PushStyleVar(style, val);
        }

        const auto res = ImGui::Begin(getComponentName().c_str(), &_isEnabled, _windowFlags);

        ImGui::PopStyleVar(static_cast<int>(_styles.size()));

        return res;
    }

    void BaseFloatEWC::endWindowDraw()
    {
        ImGui::End();
    }

    void BaseFloatEWC::onInit()
    {
        AbstractComponent::onInit();
    }

    // ========================================================================
    //
    //     _   _  _                                      _
    //    | | | |(_)                                    | |
    //    | | | | _   ___ __      __ _ __    ___   _ __ | |_
    //    | | | || | / _ \\ \ /\ / /| '_ \  / _ \ | '__|| __|
    //    \ \_/ /| ||  __/ \ V  V / | |_) || (_) || |   | |_
    //     \___/ |_| \___|  \_/\_/  | .__/  \___/ |_|    \__|
    //                              | |
    //                              |_|
    // ========================================================================
    void GameViewportEWC::onInit()
    {
        BaseFloatEWC::onInit();
    }

    void GameViewportEWC::onDraw()
    {
        if (gameInstance->renderMode.cast() == GameInstance::RenderMode::Editor)
        {
            const auto& r = gameInstance->renderToTextureObject;
            const ImVec2 renderSize = { static_cast<float>(r.getRenderSize().width),
                                        static_cast<float>(r.getRenderSize().height) };
            ImGui::Image(r.getTextureId(), renderSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
        }
    }

    // ========================================================================
    //
    //     _____  _                   _                 _
    //    /  ___|| |                 | |               | |
    //    \ `--. | |__    ___   _ __ | |_   ___  _   _ | |_  ___
    //     `--. \| '_ \  / _ \ | '__|| __| / __|| | | || __|/ __|
    //    /\__/ /| | | || (_) || |   | |_ | (__ | |_| || |_ \__ \
    //    \____/ |_| |_| \___/ |_|    \__| \___| \__,_| \__||___/
    // ========================================================================
    void KeyboardShortcutsEWC::onInit()
    {
        BaseFloatEWC::onInit();
    }

    void KeyboardShortcutsEWC::onDraw()
    {
        ImGui::Text("Shortcuts: ");
        ImGui::Text("    F1      - Toggle render mode");
        ImGui::Text("    W/A/S/D - Move Control");
        ImGui::Text("    C/Space - Down/Up");
        ImGui::Text("    Alt     - Hold to suppress a mouse");
        ImGui::Text("    F12     - exit");
    }

    // ========================================================================
    //
    //    ______                _   ______               _
    //    | ___ \              | |  |  _  \             | |
    //    | |_/ /  ___    ___  | |_ | | | |  ___    ___ | | __
    //    |    /  / _ \  / _ \ | __|| | | | / _ \  / __|| |/ /
    //    | |\ \ | (_) || (_) || |_ | |/ / | (_) || (__ |   <
    //    \_| \_| \___/  \___/  \__||___/   \___/  \___||_|\_\
    // ========================================================================
    void RootDockWindow::onInit()
    {
        BaseEWC::onInit();

        _windowFlags |= ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
                        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    void RootDockWindow::onDraw()
    {
        ImGuiID dockSpaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }

    bool RootDockWindow::beginWindowDraw()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        const auto res = ImGui::Begin(getComponentName().c_str(), &_isEnabled, _windowFlags);

        ImGui::PopStyleVar(2);

        return res;
    }

    void RootDockWindow::endWindowDraw()
    {
        ImGui::End();
    }

    // ========================================================================
    //
    //     _
    //    | |
    //    | |     ___   __ _ ___
    //    | |    / _ \ / _` / __|
    //    | |___| (_) | (_| \__ \
    //    \_____/\___/ \__, |___/
    //                  __/ |
    //                 |___/
    // ========================================================================
    void LogsWindow::addLog(Core::StringAtom&& log, spdlog::level::level_enum level)
    {
        LogLine l;
        l.message = std::move(log);
        l.level = level;
        l.hashLog = Core::StringAtom("##") + static_cast<int>(_logs.size());

        _logs.push_back(std::move(l));
    }

    void LogsWindow::clearLogs()
    {
        _logs.clear();
    }

    void LogsWindow::onInit()
    {
        BaseFloatEWC::onInit();

        _clearButtonWidth = ImGui::CalcTextSize(ICON_FA_TRASH).x;
        _autoScrollButtonWidth = ImGui::CalcTextSize(ICON_FA_ARROW_DOWN).x;
    }

    void LogsWindow::onDraw()
    {
        detectManualScroll();
        toolbarDraw();
        logsDraw();
    }

    void LogsWindow::onUpdate()
    {
        BaseFloatEWC::onUpdate();

        fetchLogs();
    }

    void LogsWindow::fetchLogs()
    {
        auto& q = LogQueue::instance();
        while (!q.isEmpty())
        {
            auto qLog = q.frontAndPop();

            addLog(qLog.toString(), qLog.level);
        }
    }

    void LogsWindow::logsDraw()
    {
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), 0,
                              ImGuiWindowFlags_HorizontalScrollbar))
        {
            bool justAdded = (_lastCountOfLogs != _logs.size());
            if (!_isAutoScroll)
            {
                justAdded = false;
            }
            _lastCountOfLogs = _logs.size();

            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear"))
                {
                    clearLogs();
                }
                ImGui::EndPopup();
            }

            std::size_t i = 0;
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten _spacing
            for (auto& [message, level, hashLog] : _logs)
            {
                std::optional<ImVec4> color;

                if (level == spdlog::level::critical)
                {
                    color = ColorRed;
                }
                if (level == spdlog::level::err)
                {
                    color = ColorYellow;
                }
                if (level == spdlog::level::warn)
                {
                    color = ColorHalfYellow;
                }
                if (level == spdlog::level::debug)
                {
                    color = ColorGrey;
                }

                if (color)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, *color);
                }

                ImGui::PushItemWidth(-FLT_MIN); // Makes the next widget take full width
                ImGui::InputText(hashLog.c_str(), message.data(), message.size() + 1,
                                 ImGuiInputTextFlags_ReadOnly);
                ImGui::PopItemWidth();

                if (color)
                {
                    ImGui::PopStyleColor();
                }

                if (justAdded && i + 1 == _logs.size())
                {
                    ImGui::SetScrollHereY(1.0f);
                }
                ++i;
            }

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
    }

    void LogsWindow::toolbarDraw()
    {
        auto startY = ImGui::GetCursorScreenPos().y;
        ImGui::BeginChild("Toolbar", ImVec2(0, _streamingToolbarHeight));
        {
            ImGui::Dummy(ImVec2(0, 0));

            // =============== Input ====================
            ImGui::Dummy(ImVec2(0, 0));
            ImGui::SameLine();
            static char filterBuf[512] = {};
            ImGui::SetNextItemWidth(_innerSize.width * 0.6f);
            ImGui::InputTextWithHint("##LogFilter",
                                     "Your filter message. Feel free to use regex(perl).",
                                     filterBuf, IM_ARRAYSIZE(filterBuf));
            ImGui::SameLine();

            // =============== Input ====================
            if (ImGui::Button(ICON_FA_TRASH))
            {
                clearLogs();
            }
            ImGui::SameLine();

            // =============== AutoScroll ====================
            if (ToggleButton(ICON_FA_ARROW_DOWN, _isAutoScroll))
            {
                _isAutoScroll = !_isAutoScroll;
            }
        }
        _streamingToolbarHeight = ImGui::GetCursorScreenPos().y - startY;
        ImGui::EndChild();
    }
    void LogsWindow::detectManualScroll()
    {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)
            && ImGui::GetIO().MouseWheel != 0.0f)
        {
            _isAutoScroll = false;
        }
    }

    // ========================================================================
    //
    //     _____  _        _              _   ______
    //    |  _  || |      (_)            | |  | ___ \
    //    | | | || |__     _   ___   ___ | |_ | |_/ / _ __   ___   _ __   ___
    //    | | | || '_ \   | | / _ \ / __|| __||  __/ | '__| / _ \ | '_ \ / __|
    //    \ \_/ /| |_) |  | ||  __/| (__ | |_ | |    | |   | (_) || |_) |\__ \
    //     \___/ |_.__/   | | \___| \___| \__|\_|    |_|    \___/ | .__/ |___/
    //                   _/ |                                     | |
    //                  |__/                                      |_|
    // ========================================================================
    void ObjectPropertiesWindow::setTargetObject(AbstractComponent* actor)
    {
        _target = actor;
    }

    void ObjectPropertiesWindow::resetTargetObject()
    {
        _target = nullptr;
    }

    void ObjectPropertiesWindow::onInit()
    {
        BaseFloatEWC::onInit();

        _transformLocationControl.components = { Vec3Control::Component{ "X:"_atom, ColorRed },
                                                 Vec3Control::Component{ "Y:"_atom, ColorGreen },
                                                 Vec3Control::Component{ "Z:"_atom, ColorBlue } };
        _transformLocationControl.labelWidth = _labelWidth;
        _transformLocationControl.label = "Location:";

        _transformOriginControl = _transformLocationControl;
        _transformOriginControl.label = "Origin:";

        _transformScaleControl = _transformLocationControl;
        _transformScaleControl.label = "Scale:";

        _transformRotationControl = _transformLocationControl;
        _transformRotationControl.label = "Rotation:";

        _meshSizeControl.components = { Vec3Control::Component{ "W:"_atom, ColorRed },
                                        Vec3Control::Component{ "H:"_atom, ColorGreen },
                                        Vec3Control::Component{ "D:"_atom, ColorBlue } };
        _meshSizeControl.labelWidth = _labelWidth;
        _meshSizeControl.readOnly = true;
        _meshSizeControl.label = "Size:";

        _frameSizeControl.components = { Vec2Control::Component{ "W:"_atom, ColorRed },
                                         Vec2Control::Component{ "H:"_atom, ColorGreen } };
        _frameSizeControl.labelWidth = _labelWidth;
        _frameSizeControl.label = "Frame size:";

        _modifierValueVec = GraphicsComponentData::ModifiedValueAsVector();
        _modifierVec = GraphicsComponentData::ModifierAsVector();

        _modifierValueRaw.clear();
        for (auto&& value : _modifierValueVec)
        {
            for (auto c : value)
            {
                _modifierValueRaw.push_back(c);
            }
            _modifierValueRaw.push_back('\0');
        }

        _modifierRaw.clear();
        for (auto&& value : _modifierVec)
        {
            for (auto c : value)
            {
                _modifierRaw.push_back(c);
            }
            _modifierRaw.push_back('\0');
        }
    }

    void ObjectPropertiesWindow::onDraw()
    {
        // A lot of stuff that should be centered or fitted.
        // Temporary disable it
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, _overriddenSpacing);

        auto* asBaseComponent = dynamic_cast<BaseComponent*>(_target);
        auto* asTransformable = dynamic_cast<Transformable*>(_target);
        auto* asStaticMeshBundle = dynamic_cast<StaticMeshBundle*>(_target);
        auto* asGraphicsComponentData = dynamic_cast<GraphicsComponentData*>(_target);
        auto* asBaseCamera = dynamic_cast<BaseCamera*>(_target);

        tryDrawBaseComponent(asBaseComponent);
        tryDrawTransformable(asTransformable);
        tryDrawGraphicsComponentData(asGraphicsComponentData);
        tryDrawStaticMeshBundle(asStaticMeshBundle);
        tryDrawBaseCamera(asBaseCamera);
        tryDrawBaseComponentExtra(asBaseComponent);

        ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing
    }

    void ObjectPropertiesWindow::onUpdate()
    {
        BaseFloatEWC::onUpdate();

        _slowUpdater.startOrUpdate();
    }

    void ObjectPropertiesWindow::tryDrawTransformable(Transformable* comp)
    {
        if (comp && ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (auto res
                = _transformLocationControl.drawAndProcess(comp->getPosition(), _innerSize.width))
            {
                comp->setPosition(GPos3(res.value()));
            }
            if (auto res
                = _transformRotationControl.drawAndProcess(comp->getRotation(), _innerSize.width))
            {
                comp->setRotation(res.value());
            }
            if (auto res
                = _transformOriginControl.drawAndProcess(comp->getOrigin(), _innerSize.width))
            {
                comp->setOrigin(res.value());
            }
            if (auto res
                = _transformScaleControl.drawAndProcess(comp->getScale(), _innerSize.width))
            {
                comp->setScale(res.value());
            }

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindow::tryDrawBaseComponent(BaseComponent* comp)
    {
        if (comp && ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
        {
            LabelAndInputTextRO("Name:", comp->getComponentName(), _labelWidth, _innerSize.width);

            LabelAndInputTextRO("Type:", comp->getComponentType(), _labelWidth, _innerSize.width);

            bool isEnabled = comp->isEnabled();
            FixedLabel("Enabled:", _labelWidth);
            ImGui::Checkbox("##isEnabled", &isEnabled);

            if (isEnabled != comp->isEnabled())
            {
                comp->setEnabled(isEnabled);
            }

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindow::tryDrawGraphicsComponentData(GraphicsComponentData* comp)
    {
        if (comp && ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            LabelAndInputTextRO("Triangles:", Core::StringAtom::MakeFrom(comp->getTriangleCount()),
                                _labelWidth, _innerSize.width);

            auto shaderName = ""_atom;
            if (comp->getShaderId())
            {
                shaderName = comp->getShaderId()->getName();
            }
            LabelAndInputTextRO("Shader: ", std::move(shaderName), _labelWidth, _innerSize.width);

            if (auto* asStaticMesh = dynamic_cast<StaticMesh*>(_target))
            {
                _meshSizeControl.drawAndProcess(asStaticMesh->getSize().toGlm(), _innerSize.width);
            }

            ImGui::Separator();
            FixedLabel("Modifiers:", _labelWidth);
            ImGui::Dummy(ImVec2(0, 0));
            static const auto collection = []()
            {
                auto c = GraphicsComponentData::ModifiedValueAsVector();
                std::sort(c.begin(), c.end());
                return c;
            }();

            static const std::vector<char> modifiers = []()
            {
                std::vector<char> out;
                out.reserve(512);

                for (auto&& i : collection)
                {
                    for (char c : i)
                    {
                        out.push_back(c);
                    }
                    out.push_back('\0');
                }
                return out;
            }();

            ImGuiStyle& style = ImGui::GetStyle();
            const char* delButtonText = "X";
            const float gap = 8.f;
            const auto buttonSize
                = ImGui::CalcTextSize(delButtonText).x + style.FramePadding.x * 2.f;
            const auto oneComboSize
                = (_innerSize.width - _labelWidth - buttonSize - gap * 2.f) / 2.f;

            bool isDirty = false;
            auto drawModifiers = comp->getDrawModifiers();
            decltype(drawModifiers) newModifiers;

            std::size_t i = 0;
            for (auto _objData : drawModifiers)
            {
                std::pair<GraphicsComponentData::ModifiedValue, GraphicsComponentData::Modifier>
                    objData = _objData;
                ImGui::PushID(static_cast<int>(i));

                FixedLabel((Core::StringAtom::MakeFrom(i) + "#").c_str(), _labelWidth);

                const int originalMod = getIndexFromModifier(objData.second);
                const int originalValueMod = getIndexFromModifier(objData.first);

                int currentMod = originalMod;
                int currentValueMod = originalValueMod;
                ImGui::PushItemWidth(oneComboSize);
                VectorCombo(("##ModifierVec" + Core::StringAtom::MakeFrom(i)).c_str(), &currentMod,
                            _modifierVec);
                ImGui::SameLine(0, gap);

                VectorCombo(("##ModifierValueVec" + Core::StringAtom::MakeFrom(i)).c_str(),
                            &currentValueMod, _modifierValueVec);
                ImGui::PopItemWidth();

                if (originalValueMod != currentValueMod)
                {
                    auto newValue
                        = GraphicsComponentData::FromString(_modifierValueVec.at(currentValueMod));

                    objData.first = newValue;
                    isDirty = true;
                }
                if (originalMod != currentMod)
                {
                    const auto newValue = GraphicsComponentData::Modifier::fromStr(
                        _modifierVec.at(currentMod).toStdString());
                    if (Verify(newValue.has_value()))
                    {
                        objData.second = newValue.value();
                        isDirty = true;
                    }
                }

                ImGui::SameLine(0, gap);

                ImGui::PushItemWidth(buttonSize);
                if (ImGui::ButtonEx(delButtonText))
                {
                    isDirty = true;
                }
                else
                {
                    newModifiers.push_back(objData);
                }
                ImGui::PopItemWidth();

                ImGui::PopID();
                ++i;
            }

            ImGui::PushItemWidth(_labelWidth);
            if (ImGui::ButtonEx("Add new item"))
            {
                newModifiers.emplace_back(GraphicsComponentData::ModifiedValue::MV_None,
                                          GraphicsComponentData::Modifier::Disable);
                isDirty = true;
            }
            ImGui::PopItemWidth();

            if (isDirty)
            {
                comp->setDrawModifiers(std::move(newModifiers));
            }

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindow::tryDrawBaseComponentExtra(BaseComponent* comp)
    {
        if (comp && ImGui::CollapsingHeader("Component data"))
        {
            LabelAndInputTextRO("Children:", comp->getChildrenCount(), _labelWidth,
                                _innerSize.width);
            LabelAndCheckboxRO("Inited:", comp->isInited(), _labelWidth);

            bool tickable = comp->getNoTick();
            FixedLabel("No ticks:", _labelWidth);
            ImGui::Checkbox("##NoTick", &tickable);
            if (comp->getNoTick() != tickable)
            {
                comp->setNoTick(tickable);
            }

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindow::tryDrawBaseCamera(BaseCamera* comp)
    {
        if (comp && ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            float inputedFov = comp->getFov();
            LabelAndInputFloat("FOV:", inputedFov, _labelWidth, _innerSize.width, 0.1f);
            if (inputedFov != comp->getFov())
            {
                comp->setFov(inputedFov);
            }

            float inputedFar = comp->getFar();
            LabelAndInputFloat("Far:", inputedFar, _labelWidth, _innerSize.width, 1.f, 0.0001f);
            if (inputedFar != comp->getFar())
            {
                comp->setFar(inputedFar);
            }

            float inputedNear = comp->getNear();
            LabelAndInputFloat("Near:", inputedNear, _labelWidth, _innerSize.width, 0.1f, 0.0001f);
            if (inputedNear != comp->getNear())
            {
                comp->setNear(inputedNear);
            }

            if (auto res
                = _frameSizeControl.drawAndProcess(comp->getFrameSize().toGlm(), _innerSize.width))
            {
                comp->setFrameSize(Core::FSize2(res.value()));
            }

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }
    }

    void ObjectPropertiesWindow::tryDrawStaticMeshBundle(StaticMeshBundle* comp)
    {
        if (comp && ImGui::CollapsingHeader("Static mesh bundle", ImGuiTreeNodeFlags_DefaultOpen))
        {
            LabelAndInputTextRO("Sub-render:", comp->getRenderTargetsCount(), _labelWidth,
                                _innerSize.width);

            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }
    }

    int ObjectPropertiesWindow::getIndexFromModifier(GraphicsComponentData::ModifiedValue v) const
    {
        for (std::size_t i = 0; i < _modifierValueVec.size(); ++i)
        {
            if (GraphicsComponentData::ToString(v) == _modifierValueVec.at(i))
            {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    int ObjectPropertiesWindow::getIndexFromModifier(GraphicsComponentData::Modifier v) const
    {
        for (std::size_t i = 0; i < _modifierVec.size(); ++i)
        {
            if (v.toStr() == _modifierVec.at(i))
            {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    // ========================================================================
    //
    // ______                    ___  ___                     ______
    // | ___ \                   |  \/  |                     | ___ \
    // | |_/ /  __ _  ___   ___  | .  . |  ___  _ __   _   _  | |_/ /  __ _  _ __
    // | ___ \ / _` |/ __| / _ \ | |\/| | / _ \| '_ \ | | | | | ___ \ / _` || '__|
    // | |_/ /| (_| |\__ \|  __/ | |  | ||  __/| | | || |_| | | |_/ /| (_| || |
    // \____/  \__,_||___/ \___| \_|  |_/ \___||_| |_| \__,_| \____/  \__,_||_|
    // ========================================================================
    void BaseMenuBarEWC::onInit()
    {
        BaseEWC::onInit();
    }

    bool BaseMenuBarEWC::beginWindowDraw()
    {
        return ImGui::BeginMainMenuBar();
    }

    void BaseMenuBarEWC::endWindowDraw()
    {
        ImGui::EndMainMenuBar();
    }

    // ========================================================================
    //
    //     _____                           _____
    //    /  ___|                         |_   _|
    //    \ `--.   ___   ___  _ __    ___   | |   _ __   ___   ___
    //     `--. \ / __| / _ \| '_ \  / _ \  | |  | '__| / _ \ / _ \
    //    /\__/ /| (__ |  __/| | | ||  __/  | |  | |   |  __/|  __/
    //    \____/  \___| \___||_| |_| \___|  \_/  |_|    \___| \___|
    // ========================================================================
    void SceneTreeWindow::onInit()
    {
        BaseFloatEWC::onInit();

        setScene(&gameInstance->gameScene);
    }

    void SceneTreeWindow::onDraw()
    {
        std::string sceneName;
        int extraFlag = ImGuiInputTextFlags_ReadOnly;
        if (_scene)
        {
            sceneName = _scene->getSceneName();
            extraFlag = 0;
        }

        ImGui::TextUnformatted("Name: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN); // Makes the next widget take full width
        ImGui::InputText("##sceneName", &sceneName, extraFlag);
        ImGui::PopItemWidth();

        if (!_scene)
        {
            return;
        }

        auto& graphicNodes = _scene->getStaticMeshBundles();
        if (ImGui::TreeNodeEx("Graphic nodes", ImGuiTreeNodeFlags_DefaultOpen | _commonTreeFlags))
        {
            int32_t internalId = 0;
            for (auto& node : graphicNodes)
            {
                drawTreeNode(&node, internalId++);
            }
            ImGui::TreePop();
        }

        auto& logicalNodes = _scene->getLogicalComponents();
        if (ImGui::TreeNodeEx("Logical nodes", ImGuiTreeNodeFlags_DefaultOpen | _commonTreeFlags))
        {
            int32_t internalId = 0;
            for (auto& node : logicalNodes)
            {
                drawTreeNode(node.get(), internalId++);
            }
            ImGui::TreePop();
        }

        if (sceneName != _scene->getSceneName())
        {
            _scene->setSceneName(sceneName.c_str());
        }
    }

    void SceneTreeWindow::onUpdate()
    {
        BaseFloatEWC::onUpdate();
    }

    void SceneTreeWindow::drawTreeNode(BaseComponent* n, int32_t id,
                                       bool isInSelectedSubtree /* = false*/)
    {
        if (!n)
        {
            return;
        }

        int flags = _commonTreeFlags | ImGuiTreeNodeFlags_OpenOnArrow;

        if (selectedObject == n && !isInSelectedSubtree)
        {
            isInSelectedSubtree = true;
        }

        if (isInSelectedSubtree)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        if (n->getChildrenCount() == 0)
        {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        ImGui::PushID(id);
        const bool isOpened = ImGui::TreeNodeEx(n->getComponentName().c_str(), flags);

        if (ImGui::IsItemClicked() || ImGui::IsItemFocused())
        {
            selectedObject = n;
            if (auto* wnd = gameInstance->gameEditor.getWindow<ObjectPropertiesWindow>())
            {
                if (wnd->isEnabled())
                {
                    wnd->setTargetObject(selectedObject);
                }
            }
        }

        if (isOpened)
        {
            for (auto&& child : n->getChildren())
            {
                drawTreeNode(child.get(), ++id);
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }
} // namespace SW