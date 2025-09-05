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

#include "GameEditor.h"

#include "Editor/Windows/AssetsManagerWindow.h"
#include "Editor/Windows/EditorMenuBarWindow.h"
#include "Editor/Windows/EditorSettings.h"
#include "Editor/Windows/GameViewport.h"
#include "Editor/Windows/LogsWindow.h"
#include "Editor/Windows/ObjectPropertiesWindow.h"
#include "Editor/Windows/RootDockWindow.h"
#include "Editor/Windows/SceneTreeWindow.h"
#include "Editor/Windows/ShaderManager.h"
#include "Editor/Windows/TextEditor.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "Misc/IconsFontAwesome.h"

using namespace Core;

namespace
{

    StringAtom GetGlslVersionShaderLike()
    {
        StringAtom version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        if (const auto* const end = version.find(" "); Verify(end))
        {
            const auto i = end - version.c_str();
            version.subStr(0, i);
            version.replaceAll(".", "");
            version.trim(' ');
            return version.push_front("#version ");
        }

        return "#version 430";
    }
} // namespace

namespace Core
{

    GameEditor::~GameEditor()
    {
        destroy();
    }

    void GameEditor::initialize()
    {
        setupImGuiStyles();

        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = "configs/windows.ini";
        io.ConfigFlags |= defaultIoConfigFlagImGui;

        if (std::filesystem::exists(defaultImGuiFontPath))
        {
            ImFont* font = io.Fonts->AddFontFromFileTTF(
                defaultImGuiFontPath.generic_string().c_str(), defaultImGuiFontSize);
            if (font)
            {
                ImGui::PushFont(font);
            }
            else
            {
                errorLog("Main font wasn't loaded by internal reasons. Font: "
                         + defaultImGuiFontPath.generic_string());
            }
        }
        else
        {
            errorLog("Main font wasn't loaded. Will be used default one. File not found: "
                     + defaultImGuiFontPath.generic_string());
            io.Fonts->AddFontDefault();
        }

        if (std::filesystem::exists(emojiImGuiFontPath))
        {
            ImFontConfig config;
            config.MergeMode = true;
            config.PixelSnapH = true; // often helps with icons
            static const ImWchar icon_ranges[]
                = { ICON_MIN_FA, ICON_MAX_FA, 0 }; // Font Awesome range
            const ImFont* font = io.Fonts->AddFontFromFileTTF(
                emojiImGuiFontPath.generic_string().c_str(),
                defaultImGuiFontSize * emojiImGuiFontScale, &config, icon_ranges);
            if (font)
            {
                // io.Fonts->Build();
            }
            else
            {
                errorLog("Emoji wasn't loaded by internal reasons. Font: "
                         + emojiImGuiFontPath.generic_string());
            }
        }
        else
        {
            errorLog("Emoji wasn't loaded. File not found: " + emojiImGuiFontPath.generic_string());
        }

        setupImGuiStyles();

        ImGui_ImplGlfw_InitForOpenGL(GetWindow().getRawWindow(), true);
        ImGui_ImplOpenGL3_Init(GetGlslVersionShaderLike().c_str());
        _isInitImGui = true;

        auto menuBar = registerNewWindow<EditorMenuBarWindowEWC>("Menu Bar"_atom);
        auto rootDocker = registerNewWindow<RootDockWindowEWC>("Root dock space"_atom);
        auto viewportWindow
            = registerNewWindow<GameViewportEWC>(ICON_FA_VIDEO_CAMERA + " Viewport"_atom);
        auto logsWindow = registerNewWindow<LogsWindowEWC>(ICON_FA_ALIGN_LEFT + " Logs"_atom);
        auto actorPropertiesWindow
            = registerNewWindow<ObjectPropertiesWindowEWC>(ICON_FA_COG + " Object Properties"_atom);
        auto sceneTreeWindow = registerNewWindow<SceneTreeWindowEWC>(ICON_FA_CUBES + " Scene"_atom);
        auto assetsManagerWindowWindow
            = registerNewWindow<AssetsManagerWindowEWC>(ICON_FA_FOLDER + " Assets"_atom);

        auto settingsWindow = registerNewWindow<EditorSettingsEWC>("Settings"_atom, true);
        auto textEditor = registerNewWindow<TextEditorEWC>("Text editor"_atom, false);
        auto shaderManager = registerNewWindow<ShaderManagerEWC>("Shader manager"_atom);

        viewportWindow->onSizeChanged.subscribe(
            [](auto outer, auto inner)
            {
                {
                    if (gGameInstance->renderMode.cast() == GameInstance::RenderMode::Editor)
                    {
                        gGameInstance->renderToTextureObject.setRenderSize(
                            static_cast<ISize2>(inner));
                        gGameInstance->updateViewport();
                    }
                }
            });

        setupShortcuts();
    }

    void GameEditor::tick(float delta)
    {
        if (_isEnabled && needToDraw())
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            for (auto&& wnd : _windows)
            {
                wnd->tick(delta);
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            _keyboardInput.update();
            _mouseInput.update();
        }
    }

    void GameEditor::destroy()
    {
        if (_isInitImGui)
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
        }
    }

    void GameEditor::setupImGuiStyles()
    {
        ImGuiStyle* style = &ImGui::GetStyle();
        if (!Verify(style))
        {
            errorLog("Can't setup ImGUI styles. ImGui::GetStyle() return nullptr");
            return;
        }

        glm::vec4* colors = style->Colors;
        if (!Verify(colors))
        {
            errorLog("Can't setup ImGUI colors. ImGui::GetStyle()->Colors return nullptr");
            return;
        }

        // Base colors for a pleasant and modern dark theme with dark accents
        colors[ImGuiCol_Text]
            = glm::vec4(0.92f, 0.93f, 0.94f, 1.00f); // Light grey text for readability
        colors[ImGuiCol_TextDisabled]
            = glm::vec4(0.56f, 0.60f, 0.56f, 1.00f); // Subtle grey for disabled text
        colors[ImGuiCol_WindowBg]
            = glm::vec4(0.14f, 0.14f, 0.16f, 1.00f); // Dark background with a hint of blue
        colors[ImGuiCol_ChildBg]
            = glm::vec4(0.16f, 0.16f, 0.18f, 1.00f); // Slightly lighter for child elements
        colors[ImGuiCol_PopupBg] = glm::vec4(0.18f, 0.18f, 0.20f, 1.00f);        // Popup background
        colors[ImGuiCol_Border] = glm::vec4(0.28f, 0.29f, 0.30f, 0.60f);         // Soft border color
        colors[ImGuiCol_BorderShadow] = glm::vec4(0.00f, 0.00f, 0.00f, 0.00f);   // No border shadow
        colors[ImGuiCol_FrameBg] = glm::vec4(0.20f, 0.22f, 0.24f, 1.00f);        // Frame background
        colors[ImGuiCol_FrameBgHovered] = glm::vec4(0.22f, 0.24f, 0.26f, 1.00f); // Frame hover effect
        colors[ImGuiCol_FrameBgActive]
            = glm::vec4(0.24f, 0.26f, 0.28f, 1.00f);                      // Active frame background
        colors[ImGuiCol_TitleBg] = glm::vec4(0.14f, 0.14f, 0.16f, 1.00f); // Title background
        colors[ImGuiCol_TitleBgActive]
            = glm::vec4(0.16f, 0.16f, 0.18f, 1.00f); // Active title background
        colors[ImGuiCol_TitleBgCollapsed]
            = glm::vec4(0.14f, 0.14f, 0.16f, 1.00f); // Collapsed title background
        colors[ImGuiCol_MenuBarBg] = glm::vec4(0.20f, 0.20f, 0.22f, 1.00f);   // Menu bar background
        colors[ImGuiCol_ScrollbarBg] = glm::vec4(0.16f, 0.16f, 0.18f, 1.00f); // Scrollbar background
        colors[ImGuiCol_ScrollbarGrab]
            = glm::vec4(0.24f, 0.26f, 0.28f, 1.00f); // Dark accent for scrollbar grab
        colors[ImGuiCol_ScrollbarGrabHovered]
            = glm::vec4(0.28f, 0.30f, 0.32f, 1.00f); // Scrollbar grab hover
        colors[ImGuiCol_ScrollbarGrabActive]
            = glm::vec4(0.32f, 0.34f, 0.36f, 1.00f);                         // Scrollbar grab active
        colors[ImGuiCol_CheckMark] = glm::vec4(0.46f, 0.56f, 0.66f, 1.00f);  // Dark blue checkmark
        colors[ImGuiCol_SliderGrab] = glm::vec4(0.36f, 0.46f, 0.56f, 1.00f); // Dark blue slider grab
        colors[ImGuiCol_SliderGrabActive]
            = glm::vec4(0.40f, 0.50f, 0.60f, 1.00f);                            // Active slider grab
        colors[ImGuiCol_Button] = glm::vec4(0.24f, 0.34f, 0.44f, 1.00f);        // Dark blue button
        colors[ImGuiCol_ButtonHovered] = glm::vec4(0.28f, 0.38f, 0.48f, 1.00f); // Button hover effect
        colors[ImGuiCol_ButtonActive] = glm::vec4(0.32f, 0.42f, 0.52f, 1.00f);  // Active button
        colors[ImGuiCol_Header]
            = glm::vec4(0.24f, 0.34f, 0.44f, 1.00f); // Header color similar to button
        colors[ImGuiCol_HeaderHovered] = glm::vec4(0.28f, 0.38f, 0.48f, 1.00f); // Header hover effect
        colors[ImGuiCol_HeaderActive] = glm::vec4(0.32f, 0.42f, 0.52f, 1.00f);  // Active header
        colors[ImGuiCol_Separator] = glm::vec4(0.28f, 0.29f, 0.30f, 1.00f);     // Separator color
        colors[ImGuiCol_SeparatorHovered]
            = glm::vec4(0.46f, 0.56f, 0.66f, 1.00f); // Hover effect for separator
        colors[ImGuiCol_SeparatorActive] = glm::vec4(0.46f, 0.56f, 0.66f, 1.00f); // Active separator
        colors[ImGuiCol_ResizeGrip] = glm::vec4(0.36f, 0.46f, 0.56f, 1.00f);      // Resize grip
        colors[ImGuiCol_ResizeGripHovered]
            = glm::vec4(0.40f, 0.50f, 0.60f, 1.00f); // Hover effect for resize grip
        colors[ImGuiCol_ResizeGripActive]
            = glm::vec4(0.44f, 0.54f, 0.64f, 1.00f);                           // Active resize grip
        colors[ImGuiCol_Tab] = glm::vec4(0.20f, 0.22f, 0.24f, 1.00f);          // Inactive tab
        colors[ImGuiCol_TabHovered] = glm::vec4(0.28f, 0.38f, 0.48f, 1.00f);   // Hover effect for tab
        colors[ImGuiCol_TabActive] = glm::vec4(0.24f, 0.34f, 0.44f, 1.00f);    // Active tab color
        colors[ImGuiCol_TabUnfocused] = glm::vec4(0.20f, 0.22f, 0.24f, 1.00f); // Unfocused tab
        colors[ImGuiCol_TabUnfocusedActive]
            = glm::vec4(0.24f, 0.34f, 0.44f, 1.00f);                        // Active but unfocused tab
        colors[ImGuiCol_PlotLines] = glm::vec4(0.46f, 0.56f, 0.66f, 1.00f); // Plot lines
        colors[ImGuiCol_PlotLinesHovered]
            = glm::vec4(0.46f, 0.56f, 0.66f, 1.00f); // Hover effect for plot lines
        colors[ImGuiCol_PlotHistogram] = glm::vec4(0.36f, 0.46f, 0.56f, 1.00f); // Histogram color
        colors[ImGuiCol_PlotHistogramHovered]
            = glm::vec4(0.40f, 0.50f, 0.60f, 1.00f); // Hover effect for histogram
        colors[ImGuiCol_TableHeaderBg]
            = glm::vec4(0.20f, 0.22f, 0.24f, 1.00f); // Table header background
        colors[ImGuiCol_TableBorderStrong]
            = glm::vec4(0.28f, 0.29f, 0.30f, 1.00f); // Strong border for tables
        colors[ImGuiCol_TableBorderLight]
            = glm::vec4(0.24f, 0.25f, 0.26f, 1.00f);                         // Light border for tables
        colors[ImGuiCol_TableRowBg] = glm::vec4(0.20f, 0.22f, 0.24f, 1.00f); // Table row background
        colors[ImGuiCol_TableRowBgAlt]
            = glm::vec4(0.22f, 0.24f, 0.26f, 1.00f); // Alternate row background
        colors[ImGuiCol_TextSelectedBg]
            = glm::vec4(0.24f, 0.34f, 0.44f, 0.35f); // Selected text background
        colors[ImGuiCol_DragDropTarget]
            = glm::vec4(0.46f, 0.56f, 0.66f, 0.90f);                           // Drag and drop target
        colors[ImGuiCol_NavHighlight] = glm::vec4(0.46f, 0.56f, 0.66f, 1.00f); // Navigation highlight
        colors[ImGuiCol_NavWindowingHighlight]
            = glm::vec4(1.00f, 1.00f, 1.00f, 0.70f); // Windowing highlight
        colors[ImGuiCol_NavWindowingDimBg]
            = glm::vec4(0.80f, 0.80f, 0.80f, 0.20f); // Dim background for windowing
        colors[ImGuiCol_ModalWindowDimBg]
            = glm::vec4(0.80f, 0.80f, 0.80f, 0.35f); // Dim background for modal windows

        // Style adjustments
        style->WindowPadding = glm::vec2(8.00f, 8.00f);
        style->FramePadding = glm::vec2(5.00f, 3.00f);
        style->CellPadding = glm::vec2(6.00f, 6.00f);
        style->ItemSpacing = glm::vec2(6.00f, 6.00f);
        style->ItemInnerSpacing = glm::vec2(6.00f, 6.00f);
        style->TouchExtraPadding = glm::vec2(4.00f, 4.00f);
        style->IndentSpacing = 25;
        style->ScrollbarSize = 11;
        style->GrabMinSize = 10;
        style->WindowBorderSize = 1;
        style->ChildBorderSize = 1;
        style->PopupBorderSize = 1;
        style->FrameBorderSize = 1;
        style->TabBorderSize = 1;
        style->WindowRounding = 7;
        style->ChildRounding = 4;
        style->FrameRounding = 3;
        style->PopupRounding = 4;
        style->ScrollbarRounding = 9;
        style->GrabRounding = 3;
        style->LogSliderDeadzone = 4;
        style->TabRounding = 4;
    }

    bool GameEditor::needToDraw()
    {
        return !_windows.empty();
    }

    void GameEditor::setupShortcuts()
    {
        _keyboardInput.getOrCreate("exit", Keyboard::Key::Key_F12)
            ->onPress.subscribe(
                [&](auto)
                {
                    GetWindow().close();
                });

        _keyboardInput.getOrCreate("cancel", Keyboard::Key::Key_Escape)
            ->onPress.subscribe(
                [&](auto)
                {
                    gGameInstance->objectSelectorManager.deselectAllAndClear();
                });

        auto mouseMove = _mouseInput.getOrCreate("mouseMove", Mouse::Key_Right);
        mouseMove->onDrag.subscribe(
            [this](auto delta, auto spec)
            {
                handleMouseDrag(delta, spec);
            });

        auto selectObject = _mouseInput.getOrCreate("selectObject", Mouse::Key_Left);
        selectObject->setIsRepeatable(false);
        selectObject->onMouseClick.subscribe(
            [this](auto pos, auto spec)
            {
                handleMouseClick(pos, spec);
            });
    }

    void GameEditor::handleMouseClick(glm::vec2 pos, MouseInputAction::SpecKeysState state)
    {
        if (gDragDrop.getState() == DragAndDrop::State::Dragging)
        {
            return;
        }

        slowObjectPicker.requestPick(
            [this](StaticMesh* mesh)
            {
                responseOnPick(mesh);
            });
    }

    void GameEditor::responseOnPick(StaticMesh* mesh)
    {
        if (const auto* wnd = gGameInstance->gameEditor.getWindow<GameViewportEWC>();
            !wnd || !wnd->isHovered())
        {
            return;
        }

        if (!mesh)
        {
            return;
        }

        if (auto* bundle = mesh->tryToGetRootBundle())
        {
            if (!bundle->isIgnoreSelect())
            {
                gGameInstance->objectSelectorManager.selectObject(bundle);
            }
            bundle->onMousePicked(mesh);
        }
        else
        {
            gGameInstance->objectSelectorManager.selectObject(mesh);
        }
    }

    void GameEditor::handleMouseDrag(glm::vec2 delta, MouseInputAction::SpecKeysState state)
    {
    }

} // namespace Core