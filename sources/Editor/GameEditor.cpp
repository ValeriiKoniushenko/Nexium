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
#include "Windows/ImageViewer.h"
#include "Windows/ModalPopUp.h"

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

        ImGui_ImplGlfw_InitForOpenGL(GetWindow().getRawWindow(), true);
        ImGui_ImplOpenGL3_Init(GetGlslVersionShaderLike().c_str());
        _isInitImGui = true;

        registerNewWindow<EditorMenuBarWindowEWC>("Menu Bar"_atom);
        registerNewWindow<RootDockWindowEWC>("Root dock space"_atom);
        registerNewWindow<LogsWindowEWC>("Logs"_atom);
        registerNewWindow<ObjectPropertiesWindowEWC>("Object Properties"_atom);
        registerNewWindow<SceneTreeWindowEWC>("Scene"_atom);
        registerNewWindow<AssetsManagerWindowEWC>("Assets"_atom);
        registerNewWindow<EditorSettingsEWC>("Settings"_atom, true);
        registerNewWindow<TextEditorEWC>("Text editor"_atom, false);
        registerNewWindow<ImageViewerEWC>("Image viewer"_atom, false);
        registerNewWindow<ShaderManagerEWC>("Shader manager"_atom);
        registerNewWindow<ModalPopUp>("PopUp"_atom);

        registerNewWindow<GameViewportEWC>("Viewport"_atom)
            ->onSizeChanged.subscribe(
                [](auto outer, auto inner)
                {
                    if (gGameInstance->renderMode.cast() == GameInstance::RenderMode::Editor)
                    {
                        gGameInstance->renderToTextureObject.setRenderSize(
                            static_cast<ISize2>(inner));
                        gGameInstance->updateViewport();
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

            keyboardInput.update();
            mouseInput.update();
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
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = "configs/windows.ini";
        io.ConfigFlags |= Config::defaultEditorImGuiFlags;

        if (std::filesystem::exists(Config::Path::editorFont))
        {
            ImFont* font = io.Fonts->AddFontFromFileTTF(
                Config::Path::editorFont.generic_string().c_str(), Config::defaultEditorFontSize);
            if (font)
            {
                ImGui::PushFont(font);
            }
            else
            {
                errorLog("Main font wasn't loaded by internal reasons. Font: "
                         + Config::Path::editorFont.generic_string());
            }
        }
        else
        {
            errorLog("Main font wasn't loaded. Will be used default one. File not found: "
                     + Config::Path::editorFont.generic_string());
            io.Fonts->AddFontDefault();
        }

        if (std::filesystem::exists(Config::Path::fontAwesome))
        {
            ImFontConfig config;
            config.MergeMode = true;
            config.PixelSnapH = true; // often helps with icons
            static constexpr ImWchar iconRanges[]
                = { ICON_MIN_FA, ICON_MAX_FA, 0 }; // Font Awesome range
            const ImFont* font = io.Fonts->AddFontFromFileTTF(
                Config::Path::fontAwesome.generic_string().c_str(),
                Config::defaultEditorFontSize * Config::defaultEmojiScale, &config, iconRanges);
            if (!font)
            {
                errorLog("Emoji wasn't loaded by internal reasons. Font: "
                         + Config::Path::fontAwesome.generic_string());
            }
        }
        else
        {
            errorLog("Emoji wasn't loaded. File not found: "
                     + Config::Path::fontAwesome.generic_string());
        }

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

        // clang-format off
        colors[ImGuiCol_Text]                  = Color4(234, 237, 239, 255).toNorm(); // Light grey text for readability
        colors[ImGuiCol_TextDisabled]          = Color4(142, 153, 142, 255).toNorm(); // Subtle grey for disabled text
        colors[ImGuiCol_WindowBg]              = Color4(35, 35, 40, 255).toNorm(); // Dark background with a hint of blue
        colors[ImGuiCol_ChildBg]               = Color4(40, 40, 45, 255).toNorm(); // Slightly lighter for child elements
        colors[ImGuiCol_PopupBg]               = Color4(45, 45, 51, 255).toNorm(); // Popup background
        colors[ImGuiCol_Border]                = Color4(71, 73, 76, 153).toNorm(); // Soft border color
        colors[ImGuiCol_BorderShadow]          = Color4(0, 0, 0, 0).toNorm(); // No border shadow
        colors[ImGuiCol_FrameBg]               = Color4(51, 56, 61, 255).toNorm(); // Frame background
        colors[ImGuiCol_FrameBgHovered]        = Color4(56, 61, 66, 255).toNorm(); // Frame hover effect
        colors[ImGuiCol_FrameBgActive]         = Color4(61, 66, 71, 255).toNorm(); // Active frame background
        colors[ImGuiCol_TitleBg]               = Color4(35, 35, 40, 255).toNorm(); // Title background
        colors[ImGuiCol_TitleBgActive]         = Color4(40, 40, 45, 255).toNorm(); // Active title background
        colors[ImGuiCol_TitleBgCollapsed]      = Color4(35, 35, 40, 255).toNorm(); // Collapsed title background
        colors[ImGuiCol_MenuBarBg]             = Color4(51, 51, 56, 255).toNorm(); // Menu bar background
        colors[ImGuiCol_ScrollbarBg]           = Color4(40, 40, 45, 255).toNorm(); // Scrollbar background
        colors[ImGuiCol_ScrollbarGrab]         = Color4(61, 66, 71, 255).toNorm(); // Dark accent for scrollbar grab
        colors[ImGuiCol_ScrollbarGrabHovered]  = Color4(71, 76, 81, 255).toNorm(); // Scrollbar grab hover
        colors[ImGuiCol_ScrollbarGrabActive]   = Color4(81, 86, 91, 255).toNorm(); // Scrollbar grab active
        colors[ImGuiCol_CheckMark]             = Color4(117, 142, 168, 255).toNorm(); // Dark blue checkmark
        colors[ImGuiCol_SliderGrab]            = Color4(91, 117, 142, 255).toNorm(); // Dark blue slider grab
        colors[ImGuiCol_SliderGrabActive]      = Color4(102, 127, 153, 255).toNorm(); // Active slider grab
        colors[ImGuiCol_Button]                = Color4(61, 86, 112, 255).toNorm(); // Dark blue button
        colors[ImGuiCol_ButtonHovered]         = Color4(71, 96, 122, 255).toNorm(); // Button hover effect
        colors[ImGuiCol_ButtonActive]          = Color4(81, 107, 132, 255).toNorm(); // Active button
        colors[ImGuiCol_Header]                = Color4(61, 86, 112, 255).toNorm(); // Header color similar to button
        colors[ImGuiCol_HeaderHovered]         = Color4(71, 96, 122, 255).toNorm(); // Header hover effect
        colors[ImGuiCol_HeaderActive]          = Color4(81, 107, 132, 255).toNorm(); // Active header
        colors[ImGuiCol_Separator]             = Color4(71, 73, 76, 255).toNorm(); // Separator color
        colors[ImGuiCol_SeparatorHovered]      = Color4(117, 142, 168, 255).toNorm(); // Hover effect for separator
        colors[ImGuiCol_SeparatorActive]       = Color4(117, 142, 168, 255).toNorm(); // Active separator
        colors[ImGuiCol_ResizeGrip]            = Color4(91, 117, 142, 255).toNorm(); // Resize grip
        colors[ImGuiCol_ResizeGripHovered]     = Color4(102, 127, 153, 255).toNorm(); // Hover effect for resize grip
        colors[ImGuiCol_ResizeGripActive]      = Color4(112, 137, 163, 255).toNorm(); // Active resize grip
        colors[ImGuiCol_Tab]                   = Color4(51, 56, 61, 255).toNorm(); // Inactive tab
        colors[ImGuiCol_TabHovered]            = Color4(71, 96, 122, 255).toNorm(); // Hover effect for tab
        colors[ImGuiCol_TabActive]             = Color4(61, 86, 112, 255).toNorm(); // Active tab color
        colors[ImGuiCol_TabUnfocused]          = Color4(51, 56, 61, 255).toNorm(); // Unfocused tab
        colors[ImGuiCol_TabUnfocusedActive]    = Color4(61, 86, 112, 255).toNorm(); // Active but unfocused tab
        colors[ImGuiCol_PlotLines]             = Color4(117, 142, 168, 255).toNorm(); // Plot lines
        colors[ImGuiCol_PlotLinesHovered]      = Color4(117, 142, 168, 255).toNorm(); // Hover effect for plot lines
        colors[ImGuiCol_PlotHistogram]         = Color4(91, 117, 142, 255).toNorm(); // Histogram color
        colors[ImGuiCol_PlotHistogramHovered]  = Color4(102, 127, 153, 255).toNorm(); // Hover effect for histogram
        colors[ImGuiCol_TableHeaderBg]         = Color4(51, 56, 61, 255).toNorm(); // Table header background
        colors[ImGuiCol_TableBorderStrong]     = Color4(71, 73, 76, 255).toNorm(); // Strong border for tables
        colors[ImGuiCol_TableBorderLight]      = Color4(61, 63, 66, 255).toNorm(); // Light border for tables
        colors[ImGuiCol_TableRowBg]            = Color4(51, 56, 61, 255).toNorm(); // Table row background
        colors[ImGuiCol_TableRowBgAlt]         = Color4(56, 61, 66, 255).toNorm(); // Alternate row background
        colors[ImGuiCol_TextSelectedBg]        = Color4(61, 86, 112, 89).toNorm(); // Selected text background
        colors[ImGuiCol_DragDropTarget]        = Color4(117, 142, 168, 229).toNorm(); // Drag and drop target
        colors[ImGuiCol_NavHighlight]          = Color4(117, 142, 168, 255).toNorm(); // Navigation highlight
        colors[ImGuiCol_NavWindowingHighlight] = Color4(255, 255, 255, 178).toNorm(); // Windowing highlight
        colors[ImGuiCol_NavWindowingDimBg]     = Color4(204, 204, 204, 51).toNorm(); // Dim background for windowing
        colors[ImGuiCol_ModalWindowDimBg]      = Color4(204, 204, 204, 89).toNorm(); // Dim background for modal windows
        // clang-format on

        // Style adjustments
        style->WindowPadding = glm::vec2(8.00f, 8.00f);
        style->FramePadding = glm::vec2(4.00f, 4.00f);
        style->CellPadding = glm::vec2(6.00f, 6.00f);
        style->ItemSpacing = glm::vec2(6.00f, 6.00f);
        style->ItemInnerSpacing = glm::vec2(6.00f, 6.00f);
        style->TouchExtraPadding = glm::vec2(4.00f, 4.00f);
        style->IndentSpacing = 25;
        style->ScrollbarSize = 10;
        style->GrabMinSize = 10;
        style->WindowBorderSize = 1;
        style->ChildBorderSize = 1;
        style->PopupBorderSize = 1;
        style->FrameBorderSize = 1;
        style->TabBorderSize = 1;
        style->LogSliderDeadzone = 4;

        constexpr float rounding = 6.f;
        style->WindowRounding = rounding * 2.f;
        style->ChildRounding = rounding;
        style->PopupRounding = rounding;
        style->FrameRounding = rounding;
        style->ScrollbarRounding = rounding * 2.f;
        style->GrabRounding = rounding;
        style->TabRounding = rounding;
        style->TreeLinesRounding = rounding;
    }

    bool GameEditor::needToDraw()
    {
        return !_windows.empty();
    }

    void GameEditor::setupShortcuts()
    {
        keyboardInput.getOrCreate("Close editor", Keyboard::Key::Key_F12)
            ->onPress.subscribe(
                [&](auto)
                {
                    GetWindow().close();
                });

        keyboardInput.getOrCreate("Cancel action", Keyboard::Key::Key_Escape)
            ->onPress.subscribe(
                [&](auto)
                {
                    gGameInstance->objectSelectorManager.deselectAllAndClear();
                });

        auto mouseMove = mouseInput.getOrCreate("mouseMove", Mouse::Key_Right);
        mouseMove->onDrag.subscribe(
            [this](auto delta, auto spec)
            {
                handleMouseDrag(delta, spec);
            });

        auto selectObject = mouseInput.getOrCreate("selectObject", Mouse::Key_Left);
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