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

#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Misc/ImGuiHelper.h"

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

        ImGui::PopStyleVar(_styles.size());

        return res;
    }

    void BaseFloatEWC::endWindowDraw()
    {
        ImGui::End();
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
        BaseEWC::onInit();
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
        ImGui::Text("Shortcuts:");
        ImGui::Text("    F1      - Toggle render mode");
        ImGui::Text("    W/A/S/D - Move Control");
        ImGui::Text("    C/Space - Down/Up");
        ImGui::Text("    M       - Mouse capture");
        ImGui::Text("    Alt     - Hold to suppress a mouse");
        ImGui::Text("    Esc     - exit");
        ImGui::Text("");
        ImGui::Text("Globals:");
        ImGui::Text("Mouse sensitivity");
        // ImGui::SliderFloat("##mouseSensitivity", &gameInstance->mouseSensitivity, 0.f, 2000.f);
        ImGui::Text("Spectator speed");
        // ImGui::SliderFloat("##spectator_speed", &gameInstance->speed, 1.f, 1000.f);
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
        l.hashLog = Core::StringAtom("##") + _logs.size();

        _logs.push_back(std::move(l));
    }

    void LogsWindow::clearLogs()
    {
        _logs.clear();
    }

    void LogsWindow::onInit()
    {
        BaseFloatEWC::onInit();
    }

    void LogsWindow::onDraw()
    {
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), ImGuiChildFlags_NavFlattened,
                              ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear"))
                {
                    clearLogs();
                }
                ImGui::EndPopup();
            }

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten _spacing
            for (auto& [message, level, hashLog] : _logs)
            {
                std::optional<ImVec4> color;

                if (level == spdlog::level::critical)
                {
                    color = ImVec4(229.f / 255.f, 20.f / 255.f, 9.f / 255.f, 1.0f); // red
                }
                if (level == spdlog::level::err)
                {
                    color = ImVec4(252.f / 255.f, 148.f / 141.f, 3.f / 255.f, 1.0f); // orange
                }
                if (level == spdlog::level::warn)
                {
                    color = ImVec4(190.f / 255.f, 190.f / 141.f, 190.f / 255.f, 1.0f); // gray
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
            }

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
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

        _slowUpdater.setRepeatTime(1.f / 30.f);
        _slowUpdater.setCallback(
            [](auto)
            {

            });
    }

    void ObjectPropertiesWindow::onDraw()
    {
        // One-row structure is:
        // | Label | (ID Input) (ID Input) (ID Input) |
        // For Label - static width
        // For (ID Input):
        //    For 'ID' - static
        //    For 'Input' - dynamic
        // Spacing - static

        _fullWidth = ImGui::GetContentRegionAvail().x;
        _labelWidth = ImGui::CalcTextSize("SomeLongWord: ").x;
        _spacing = ImGui::GetStyle().ItemSpacing.x;
        constexpr float componentsCount = 3.f;      // 3 -> X Y Z
        constexpr float totalComponentsCount = 7.f; // 3 -> | Text | (ID Input) * 3 |
        _inputWidth = (_fullWidth - _labelWidth - componentsCount * ImGui::CalcTextSize("X:").x
                       - totalComponentsCount * _spacing)
                      / componentsCount;

        auto* asBaseComponent = dynamic_cast<BaseComponent*>(_target);
        auto* asTransformable = dynamic_cast<Transformable*>(_target);
        auto* asStaticMeshBundle = dynamic_cast<StaticMeshBundle*>(_target);
        auto* asGraphicsComponentData = dynamic_cast<GraphicsComponentData*>(_target);

        if (asBaseComponent && ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
        {
            LabelAndInputTextRO("Name:", asBaseComponent->getComponentName(), _labelWidth,
                                _fullWidth);

            LabelAndInputTextRO("Type:", asBaseComponent->getComponentType(), _labelWidth,
                                _fullWidth);

            bool isEnabled = asBaseComponent->isEnabled();
            FixedLabel("Enabled:", _labelWidth);
            ImGui::Checkbox("##isEnabled", &isEnabled);

            if (isEnabled != asBaseComponent->isEnabled())
            {
                asBaseComponent->setEnabled(isEnabled);
            }
        }

        ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));

        if (asTransformable && ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            glm::vec3 location = asTransformable->getPosition();
            glm::vec3 rotation = asTransformable->getRotation();
            glm::vec3 scale = asTransformable->getScale();
            glm::vec3 origin = asTransformable->getOrigin();

            constexpr auto rowSpacing = ImVec2(0.0f, 5.0f);
            drawVec3Control("Location", location);
            ImGui::Dummy(rowSpacing);
            drawVec3Control("Rotation", rotation);
            ImGui::Dummy(rowSpacing);
            drawVec3Control("Origin", origin);
            ImGui::Dummy(rowSpacing);
            drawVec3Control("Scale", scale);

            if (location != asTransformable->getPosition())
            {
                asTransformable->setPosition(GPos3(location));
            }
            if (rotation != asTransformable->getRotation())
            {
                asTransformable->setRotation(rotation);
            }
            if (scale != asTransformable->getScale())
            {
                asTransformable->setScale(scale);
            }
            if (origin != asTransformable->getOrigin())
            {
                asTransformable->setOrigin(origin);
            }
        }

        ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));

        if (asGraphicsComponentData
            && ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            LabelAndInputTextRO(
                "Triangles:",
                Core::StringAtom::MakeFrom(asGraphicsComponentData->getTriangleCount()),
                _labelWidth, _fullWidth);

            auto shaderName = ""_atom;
            if (asGraphicsComponentData->getShaderId())
            {
                shaderName = asGraphicsComponentData->getShaderId()->getName();
            }
            LabelAndInputTextRO("Shader: ", std::move(shaderName), _labelWidth, _fullWidth);

            if (auto* asStaticMesh = dynamic_cast<StaticMesh*>(_target))
            {
                const auto size = asStaticMesh->getSize();
                auto width = std::format("{:.2f}", size.width);
                auto height = std::format("{:.2f}", size.height);
                auto deep = std::format("{:.2f}", size.deep);

                FixedLabel("Size:", _labelWidth);

                ImGui::PushStyleColor(ImGuiCol_Text, COLOR_X);
                ImGui::TextUnformatted("W:");
                ImGui::PopStyleColor();
                ImGui::SameLine(0, 3.f);
                ImGui::PushItemWidth(_inputWidth);
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
                ImGui::InputText("##width", width.data(), width.size() + 1,
                                 ImGuiInputTextFlags_ReadOnly);
                ImGui::PopStyleColor();
                ImGui::PopItemWidth();
                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Text, COLOR_Y);
                ImGui::TextUnformatted("H:");
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::PushItemWidth(_inputWidth);
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
                ImGui::InputText("##height", height.data(), height.size() + 1,
                                 ImGuiInputTextFlags_ReadOnly);
                ImGui::PopStyleColor();
                ImGui::PopItemWidth();
                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Text, COLOR_Z);
                ImGui::TextUnformatted("D:");
                ImGui::SameLine();
                ImGui::PopStyleColor();
                ImGui::PushItemWidth(_inputWidth);
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
                ImGui::InputText("##deep", deep.data(), deep.size() + 1,
                                 ImGuiInputTextFlags_ReadOnly);
                ImGui::PopStyleColor();
                ImGui::PopItemWidth();
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

            /*
            static auto indexOfCollection = [](GraphicsComponentData::Modifier mod){
                auto it = std::find(collection.begin(), collection.end(), mod.toStr());
                Assert(it != collection.end());

                return std::distance(collection.begin(), it);
            };
            static const char modifierStatuses[64] = "Enable\0Disable";
            // Construction of one line is:
            // | <ID> | <Modifier>  <Value> | <DelBtn> |
            // For ID & DelBtn - fixed size
            static const auto buttonSize = ImGui::CalcTextSize("Del").x;
            const auto freeSpace = _fullWidth - _labelWidth - _spacing * 4.f - buttonSize;
            const auto oneComboSize = freeSpace / 2.f;

            _graphicsMods.resize(0);
            for (auto [status, val] : asGraphicsComponentData->getDrawModifiers())
            {
                _graphicsMods.push_back({ status, indexOfCollection(val) });
            }

            int id = 0;
            for (auto _ : _graphicsMods)
            {
                ImGui::PushID(id);
                FixedLabel((Core::StringAtom::MakeFrom(id) + "#").c_str(), _labelWidth);

                ImGui::PushItemWidth(oneComboSize);
                ImGui::Combo("##ModifierStatus", &_graphicsMods.at(id).second, modifierStatuses,
                             IM_ARRAYSIZE(modifierStatuses));
                ImGui::SameLine();
                ImGui::Combo("##ModifierValue", &_graphicsMods.at(id).first, modifiers.data(),
                             modifiers.size());
                ImGui::PopItemWidth();

                ImGui::SameLine();

                ImGui::PushItemWidth(buttonSize);
                if (ImGui::ButtonEx("Del"))
                {
                    warnLog("Hello");
                }
                ImGui::PopItemWidth();

                ImGui::PopID();
                ++id;
            }
             */
            ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));
        }

        ImGui::Dummy(ImVec2(0.0f, _gapBetweenSections));

        if (asBaseComponent && ImGui::CollapsingHeader("Component data"))
        {
            LabelAndInputTextRO("Children:", asBaseComponent->getChildrenCount(), _labelWidth,
                                _fullWidth);
            LabelAndCheckboxRO("Inited:", asBaseComponent->isInited(), _labelWidth);

            bool tickable = asBaseComponent->getNoTick();
            FixedLabel("No ticks:", _labelWidth);
            ImGui::Checkbox("##NoTick", &tickable);
            if (asBaseComponent->getNoTick() != tickable)
            {
                asBaseComponent->setNoTick(tickable);
            }
        }
    }

    void ObjectPropertiesWindow::onUpdate()
    {
        BaseFloatEWC::onUpdate();

        _slowUpdater.startOrUpdate();
    }

    void ObjectPropertiesWindow::drawVec3Control(const char* label, glm::vec3& v)
    {
        ImGui::PushID(label);

        FixedLabel(label, _labelWidth);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_X);
        ImGui::TextUnformatted("X:");
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 3.f);
        ImGui::PushItemWidth(_inputWidth);
        ImGui::DragFloat("##X", &v.x, 0.1f, 0.0f, 0.0f, "%.3f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_Y);
        ImGui::TextUnformatted("Y:");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushItemWidth(_inputWidth);
        ImGui::DragFloat("##Y", &v.y, 0.1f, 0.0f, 0.0f, "%.3f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_Z);
        ImGui::TextUnformatted("Z:");
        ImGui::SameLine();
        ImGui::PopStyleColor();
        ImGui::PushItemWidth(_inputWidth);
        ImGui::DragFloat("##Z", &v.z, 0.1f, 0.0f, 0.0f, "%.3f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::PopID();
    }

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

        auto& meshes = _scene->getStaticMeshBundles();
        if (ImGui::TreeNodeEx("Collection", ImGuiTreeNodeFlags_DefaultOpen | _commonTreeFlags))
        {
            int32_t internalId = 0;
            for (auto& mesh : meshes)
            {
                drawTreeNode(&mesh, internalId++);
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