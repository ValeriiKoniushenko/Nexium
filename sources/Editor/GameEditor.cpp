// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "GameEditor.h"

#include "Core/IntrusivePtr.h"
#include "Editor/EditorIntegration.h"
#include "Editor/IconsFontAwesome.h"
#include "Editor/Windows/GameViewport.h"
#include "Editor/Windows/RootDockWindow.h"
#include "Foundation/Configs.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "NxFundamental/ITagHolder.h"
#ifdef NEXIUM_ENABLE_3D_MODULE
    #include "NxWorld/Entities/Mesh/StaticMeshBundle.h"
#endif
#include "NxWorld/Framework/GameInstance.h"
#include "PrivateModuleInfo.h"

#include <array>
#include <exception>
#include <string>

using namespace Core;
using namespace NX;
using namespace Platform;
namespace Config = Foundation::Config;

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
            if (version.size() < 3)
            {
                version.pushBack('0');
            }

            version.pushFront("#version ");
            return version;
        }

        return "#version 430";
    }
} // namespace

namespace NX
{
    const int GameEditor::defaultEditorImGuiFlags
        = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;

    GameEditor::~GameEditor()
    {
        destroy();
    }

    void GameEditor::preInitialize()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;

        gameViewport.generate();

        setupImGuiStyles();
        setupShortcuts();

        ImGui_ImplGlfw_InitForOpenGL(GetWindow().getRawWindow(), true);
        ImGui_ImplOpenGL3_Init(GetGlslVersionShaderLike().c_str());
        _isInitImGui = true;
    }

    IntrusivePtr<BaseEWC> GameEditor::registerNewWindow(IntrusivePtr<BaseEWC> wnd,
                                                        bool isEnabled /*  = false */)
    {
        auto wndType = wnd->getComponentType();
        Assert(!wndType.isEmpty());
        if (!wndType.isEmpty() && _windowTypes.contains(wndType))
        {
            errorLogAndAssert("Such window '{}' already was registered"_f << wndType);
            return nullptr;
        }

        _windowTypes.emplace(wndType);
        auto& a = _windows.emplace_back(std::move(wnd));

        a->initialize();
        auto name = a->getComponentName();
        if (a->getIcon())
        {
            name = a->getIcon() + (" " + name);
        }
        a->setComponentName(std::move(name));

        if (isEnabled)
        {
            a->openWindow();
        }
        else
        {
            a->closeWindow();
        }

        return a;
    }

    void GameEditor::initialize()
    {
        auto windows = GetGlobalComponentFactory().getRegisteredTypesAsVector(
            false, [](Tag tag) { return tag & Tag_EditorWindow; });

        for (auto&& wnd : windows)
        {
            IntrusivePtr<BaseEWC> tmp
                = dynamic_cast<BaseEWC*>(GetGlobalComponentFactory().create(wnd));
            registerNewWindow(std::move(tmp));
        }

        if (auto* viewport = getWindow<GameViewportEWC>(); Verify(viewport))
        {
            _subscriptionPool << viewport->onSizeChanged->subscribeAndGetID(
                [](auto outer, auto inner)
                {
                    if (gGameInstance->renderMode == GameInstance::RenderMode::Editor)
                    {
                        GetEditor()->gameViewport.setRenderSize(static_cast<ISize2>(inner));
                        gGameInstance->updateViewport();
                    }
                });
        }
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
            notifications.tick(delta);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            keyboardInput.update();
            mouseInput.update();

            // The reason of this code is:
            // Nexium uses ImGUI as a Windows provider. At the start it loads all settings from
            // a corresponding .ini file using the default ImGUI approach:
            // ImGui::LoadIniSettingsFromDisk
            // The engine, for now, can't control what was loaded, what wasn't. So, we do
            // 'lazy loading' to finish-loading of all opened windows; opened by ImGUI.
            static std::once_flag winLazyInit;
            std::call_once(winLazyInit, [this]() { lazyOneShotInitialization(); });
        }
    }

    void GameEditor::destroy()
    {
        if (_isInitImGui)
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }
    }

    spdlog::logger* GameEditor::getLogger() const
    {
        return Editor::getLogger();
    }

    void GameEditor::writeToCache()
    {
        auto& cs = GetCacheSystem();
        for (auto& wnd : _windows)
        {
            cs.write(*wnd);
        }

        Editor::NotificationPopUp::Success("Changes are saved!").show();
    }

    void GameEditor::readFromCache()
    {
        auto& cs = GetCacheSystem();
        for (auto& wnd : _windows)
        {
            cs.read(*wnd);
        }
    }

    void GameEditor::setupImGuiStyles()
    {
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= GameEditor::defaultEditorImGuiFlags;

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
            static constexpr std::array<ImWchar, 3> iconRanges
                = { ICON_MIN_FA, ICON_MAX_FA, 0 }; // Font Awesome range
            const ImFont* font = io.Fonts->AddFontFromFileTTF(
                Config::Path::fontAwesome.generic_string().c_str(),
                Config::defaultEditorFontSize * Config::defaultEmojiScale, &config,
                iconRanges.data());
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

        auto& colors = style->Colors;

        constexpr int theme = 2;
        if constexpr (theme == 0) // default
        {
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
        else if constexpr (theme == 1) // more dark
        {
            // Nexium Editor Theme — "Voidwave" (violet + cyan duotone)
            // Deeper near-black base than the original slate-blue theme, with a
            // violet/cyan accent pair instead of a single muted blue. Cyan is reserved
            // for "active/confirm" signals (checkmarks, active separators, nav),
            // violet carries the interactive surfaces (buttons, sliders, tabs).

            // clang-format off
            colors[ImGuiCol_Text]                  = Color4(230, 230, 240, 255).toNorm(); // Crisp off-white text
            colors[ImGuiCol_TextDisabled]          = Color4(110, 112, 130, 255).toNorm(); // Muted cool grey
            colors[ImGuiCol_WindowBg]              = Color4(16, 17, 23, 255).toNorm(); // Near-black navy
            colors[ImGuiCol_ChildBg]               = Color4(20, 21, 28, 255).toNorm(); // Slightly lifted child bg
            colors[ImGuiCol_PopupBg]               = Color4(24, 25, 33, 255).toNorm(); // Popup background
            colors[ImGuiCol_Border]                = Color4(52, 54, 68, 140).toNorm(); // Soft cool border
            colors[ImGuiCol_BorderShadow]          = Color4(0, 0, 0, 0).toNorm(); // No border shadow
            colors[ImGuiCol_FrameBg]               = Color4(28, 29, 38, 255).toNorm(); // Frame background
            colors[ImGuiCol_FrameBgHovered]        = Color4(40, 34, 60, 255).toNorm(); // Hint of violet on hover
            colors[ImGuiCol_FrameBgActive]         = Color4(52, 40, 78, 255).toNorm(); // Stronger violet when active
            colors[ImGuiCol_TitleBg]               = Color4(16, 17, 23, 255).toNorm(); // Title background
            colors[ImGuiCol_TitleBgActive]         = Color4(24, 25, 33, 255).toNorm(); // Active title background
            colors[ImGuiCol_TitleBgCollapsed]      = Color4(16, 17, 23, 255).toNorm(); // Collapsed title background
            colors[ImGuiCol_MenuBarBg]             = Color4(22, 23, 30, 255).toNorm(); // Menu bar background
            colors[ImGuiCol_ScrollbarBg]           = Color4(20, 21, 28, 255).toNorm(); // Scrollbar background
            colors[ImGuiCol_ScrollbarGrab]         = Color4(52, 40, 78, 255).toNorm(); // Violet scrollbar grab
            colors[ImGuiCol_ScrollbarGrabHovered]  = Color4(72, 56, 112, 255).toNorm(); // Grab hover
            colors[ImGuiCol_ScrollbarGrabActive]   = Color4(92, 70, 146, 255).toNorm(); // Grab active
            colors[ImGuiCol_CheckMark]             = Color4(0, 224, 196, 255).toNorm(); // Cyan checkmark (accent #2)
            colors[ImGuiCol_SliderGrab]            = Color4(124, 92, 246, 255).toNorm(); // Violet slider grab (accent #1)
            colors[ImGuiCol_SliderGrabActive]      = Color4(150, 120, 255, 255).toNorm(); // Brighter active slider
            colors[ImGuiCol_Button]                = Color4(76, 58, 133, 255).toNorm(); // Violet button
            colors[ImGuiCol_ButtonHovered]         = Color4(96, 74, 168, 255).toNorm(); // Button hover
            colors[ImGuiCol_ButtonActive]          = Color4(116, 90, 204, 255).toNorm(); // Active button
            colors[ImGuiCol_Header]                = Color4(76, 58, 133, 255).toNorm(); // Header matches button
            colors[ImGuiCol_HeaderHovered]         = Color4(96, 74, 168, 255).toNorm(); // Header hover
            colors[ImGuiCol_HeaderActive]          = Color4(116, 90, 204, 255).toNorm(); // Active header
            colors[ImGuiCol_Separator]             = Color4(52, 54, 68, 255).toNorm(); // Separator color
            colors[ImGuiCol_SeparatorHovered]      = Color4(0, 224, 196, 255).toNorm(); // Cyan on hover — pops against violet UI
            colors[ImGuiCol_SeparatorActive]       = Color4(0, 224, 196, 255).toNorm(); // Cyan when active
            colors[ImGuiCol_ResizeGrip]            = Color4(92, 70, 146, 255).toNorm(); // Resize grip
            colors[ImGuiCol_ResizeGripHovered]     = Color4(112, 86, 178, 255).toNorm(); // Hover
            colors[ImGuiCol_ResizeGripActive]      = Color4(132, 102, 210, 255).toNorm(); // Active
            colors[ImGuiCol_Tab]                   = Color4(28, 29, 38, 255).toNorm(); // Inactive tab
            colors[ImGuiCol_TabHovered]            = Color4(96, 74, 168, 255).toNorm(); // Hover effect for tab
            colors[ImGuiCol_TabActive]             = Color4(76, 58, 133, 255).toNorm(); // Active tab color
            colors[ImGuiCol_TabUnfocused]          = Color4(24, 25, 33, 255).toNorm(); // Unfocused tab
            colors[ImGuiCol_TabUnfocusedActive]    = Color4(52, 40, 78, 255).toNorm(); // Active but unfocused tab
            colors[ImGuiCol_PlotLines]             = Color4(0, 224, 196, 255).toNorm(); // Cyan plot lines
            colors[ImGuiCol_PlotLinesHovered]      = Color4(64, 240, 216, 255).toNorm(); // Hover effect for plot lines
            colors[ImGuiCol_PlotHistogram]         = Color4(124, 92, 246, 255).toNorm(); // Violet histogram
            colors[ImGuiCol_PlotHistogramHovered]  = Color4(150, 120, 255, 255).toNorm(); // Hover effect for histogram
            colors[ImGuiCol_TableHeaderBg]         = Color4(28, 29, 38, 255).toNorm(); // Table header background
            colors[ImGuiCol_TableBorderStrong]     = Color4(52, 54, 68, 255).toNorm(); // Strong border for tables
            colors[ImGuiCol_TableBorderLight]      = Color4(38, 40, 50, 255).toNorm(); // Light border for tables
            colors[ImGuiCol_TableRowBg]            = Color4(20, 21, 28, 255).toNorm(); // Table row background
            colors[ImGuiCol_TableRowBgAlt]         = Color4(26, 27, 35, 255).toNorm(); // Alternate row background
            colors[ImGuiCol_TextSelectedBg]        = Color4(124, 92, 246, 89).toNorm(); // Violet selected text background
            colors[ImGuiCol_DragDropTarget]        = Color4(0, 224, 196, 229).toNorm(); // Cyan drag and drop target
            colors[ImGuiCol_NavHighlight]          = Color4(0, 224, 196, 255).toNorm(); // Cyan navigation highlight
            colors[ImGuiCol_NavWindowingHighlight] = Color4(255, 255, 255, 178).toNorm(); // Windowing highlight
            colors[ImGuiCol_NavWindowingDimBg]     = Color4(10, 10, 14, 102).toNorm(); // Darker dim for windowing (was mid-grey)
            colors[ImGuiCol_ModalWindowDimBg]      = Color4(10, 10, 14, 140).toNorm(); // Darker modal dim, less washed-out
            // clang-format on

            // Style adjustments — slightly airier spacing, more pronounced rounding
            // for a softer "modern flat" feel; mixed rounding sizes instead of one
            // uniform scale so big surfaces (windows) read as distinctly softer than
            // small controls (frames/tabs).
            style->WindowPadding = glm::vec2(10.00f, 10.00f);
            style->FramePadding = glm::vec2(6.00f, 5.00f);
            style->CellPadding = glm::vec2(8.00f, 6.00f);
            style->ItemSpacing = glm::vec2(8.00f, 6.00f);
            style->ItemInnerSpacing = glm::vec2(6.00f, 6.00f);
            style->TouchExtraPadding = glm::vec2(4.00f, 4.00f);
            style->IndentSpacing = 22;
            style->ScrollbarSize = 12;
            style->GrabMinSize = 12;
            style->WindowBorderSize = 1;
            style->ChildBorderSize = 1;
            style->PopupBorderSize = 1;
            style->FrameBorderSize
                = 0; // borderless frames read as more modern; color contrast carries the shape
            style->TabBorderSize = 0;
            style->LogSliderDeadzone = 4;

            constexpr float windowRounding = 12.f;
            constexpr float controlRounding = 8.f;
            style->WindowRounding = windowRounding;
            style->ChildRounding = controlRounding;
            style->PopupRounding = controlRounding;
            style->FrameRounding = controlRounding;
            style->ScrollbarRounding = windowRounding;
            style->GrabRounding = controlRounding;
            style->TabRounding = controlRounding;
            style->TreeLinesRounding = controlRounding;
        }
        else if constexpr (theme == 2) // Modifed previous
        {
            // Nexium Editor Theme — "Voidwave" (violet + cyan duotone)
            // Deeper near-black base than the original slate-blue theme, with a
            // violet/cyan accent pair instead of a single muted blue. Cyan is reserved
            // for "active/confirm" signals (checkmarks, active separators, nav),
            // violet carries the interactive surfaces (buttons, sliders, tabs).
            // WindowBg/TitleBg stay darkest as the anchor; child/frame/popup/table
            // surfaces sit one step brighter so content inside a window reads as
            // visibly "lifted" off its background.

            // clang-format off
            colors[ImGuiCol_Text]                  = Color4(230, 230, 240, 255).toNorm(); // Crisp off-white text
            colors[ImGuiCol_TextDisabled]          = Color4(110, 112, 130, 255).toNorm(); // Muted cool grey
            colors[ImGuiCol_WindowBg]              = Color4(16, 17, 23, 255).toNorm(); // Near-black navy
            colors[ImGuiCol_ChildBg]               = Color4(28, 29, 38, 255).toNorm(); // Brighter lift off WindowBg
            colors[ImGuiCol_PopupBg]               = Color4(34, 35, 46, 255).toNorm(); // Popup background
            colors[ImGuiCol_Border]                = Color4(58, 60, 76, 140).toNorm(); // Soft cool border
            colors[ImGuiCol_BorderShadow]          = Color4(0, 0, 0, 0).toNorm(); // No border shadow
            colors[ImGuiCol_FrameBg]               = Color4(38, 39, 51, 255).toNorm(); // Frame background — brighter, reads as an inset surface
            colors[ImGuiCol_FrameBgHovered]        = Color4(50, 44, 72, 255).toNorm(); // Hint of violet on hover
            colors[ImGuiCol_FrameBgActive]         = Color4(62, 50, 90, 255).toNorm(); // Stronger violet when active
            colors[ImGuiCol_TitleBg]               = Color4(16, 17, 23, 255).toNorm(); // Title background
            colors[ImGuiCol_TitleBgActive]         = Color4(24, 25, 33, 255).toNorm(); // Active title background
            colors[ImGuiCol_TitleBgCollapsed]      = Color4(16, 17, 23, 255).toNorm(); // Collapsed title background
            colors[ImGuiCol_MenuBarBg]             = Color4(30, 31, 40, 255).toNorm(); // Menu bar background
            colors[ImGuiCol_ScrollbarBg]           = Color4(28, 29, 38, 255).toNorm(); // Scrollbar background
            colors[ImGuiCol_ScrollbarGrab]         = Color4(52, 40, 78, 255).toNorm(); // Violet scrollbar grab
            colors[ImGuiCol_ScrollbarGrabHovered]  = Color4(72, 56, 112, 255).toNorm(); // Grab hover
            colors[ImGuiCol_ScrollbarGrabActive]   = Color4(92, 70, 146, 255).toNorm(); // Grab active
            colors[ImGuiCol_CheckMark]             = Color4(0, 224, 196, 255).toNorm(); // Cyan checkmark (accent #2)
            colors[ImGuiCol_SliderGrab]            = Color4(124, 92, 246, 255).toNorm(); // Violet slider grab (accent #1)
            colors[ImGuiCol_SliderGrabActive]      = Color4(150, 120, 255, 255).toNorm(); // Brighter active slider
            colors[ImGuiCol_Button]                = Color4(76, 58, 133, 255).toNorm(); // Violet button
            colors[ImGuiCol_ButtonHovered]         = Color4(96, 74, 168, 255).toNorm(); // Button hover
            colors[ImGuiCol_ButtonActive]          = Color4(116, 90, 204, 255).toNorm(); // Active button
            colors[ImGuiCol_Header]                = Color4(76, 58, 133, 255).toNorm(); // Header matches button
            colors[ImGuiCol_HeaderHovered]         = Color4(96, 74, 168, 255).toNorm(); // Header hover
            colors[ImGuiCol_HeaderActive]          = Color4(116, 90, 204, 255).toNorm(); // Active header
            colors[ImGuiCol_Separator]             = Color4(52, 54, 68, 255).toNorm(); // Separator color
            colors[ImGuiCol_SeparatorHovered]      = Color4(0, 224, 196, 255).toNorm(); // Cyan on hover — pops against violet UI
            colors[ImGuiCol_SeparatorActive]       = Color4(0, 224, 196, 255).toNorm(); // Cyan when active
            colors[ImGuiCol_ResizeGrip]            = Color4(92, 70, 146, 255).toNorm(); // Resize grip
            colors[ImGuiCol_ResizeGripHovered]     = Color4(112, 86, 178, 255).toNorm(); // Hover
            colors[ImGuiCol_ResizeGripActive]      = Color4(132, 102, 210, 255).toNorm(); // Active
            colors[ImGuiCol_Tab]                   = Color4(38, 39, 51, 255).toNorm(); // Inactive tab — brighter to match frame surfaces
            colors[ImGuiCol_TabHovered]            = Color4(96, 74, 168, 255).toNorm(); // Hover effect for tab
            colors[ImGuiCol_TabActive]             = Color4(76, 58, 133, 255).toNorm(); // Active tab color
            colors[ImGuiCol_TabUnfocused]          = Color4(32, 33, 43, 255).toNorm(); // Unfocused tab
            colors[ImGuiCol_TabUnfocusedActive]    = Color4(62, 50, 90, 255).toNorm(); // Active but unfocused tab
            colors[ImGuiCol_PlotLines]             = Color4(0, 224, 196, 255).toNorm(); // Cyan plot lines
            colors[ImGuiCol_PlotLinesHovered]      = Color4(64, 240, 216, 255).toNorm(); // Hover effect for plot lines
            colors[ImGuiCol_PlotHistogram]         = Color4(124, 92, 246, 255).toNorm(); // Violet histogram
            colors[ImGuiCol_PlotHistogramHovered]  = Color4(150, 120, 255, 255).toNorm(); // Hover effect for histogram
            colors[ImGuiCol_TableHeaderBg]         = Color4(38, 39, 51, 255).toNorm(); // Table header background
            colors[ImGuiCol_TableBorderStrong]     = Color4(58, 60, 76, 255).toNorm(); // Strong border for tables
            colors[ImGuiCol_TableBorderLight]      = Color4(44, 46, 58, 255).toNorm(); // Light border for tables
            colors[ImGuiCol_TableRowBg]            = Color4(28, 29, 38, 255).toNorm(); // Table row background
            colors[ImGuiCol_TableRowBgAlt]         = Color4(34, 35, 45, 255).toNorm(); // Alternate row background
            colors[ImGuiCol_TextSelectedBg]        = Color4(124, 92, 246, 89).toNorm(); // Violet selected text background
            colors[ImGuiCol_DragDropTarget]        = Color4(0, 224, 196, 229).toNorm(); // Cyan drag and drop target
            colors[ImGuiCol_NavHighlight]          = Color4(0, 224, 196, 255).toNorm(); // Cyan navigation highlight
            colors[ImGuiCol_NavWindowingHighlight] = Color4(255, 255, 255, 178).toNorm(); // Windowing highlight
            colors[ImGuiCol_NavWindowingDimBg]     = Color4(10, 10, 14, 102).toNorm(); // Darker dim for windowing (was mid-grey)
            colors[ImGuiCol_ModalWindowDimBg]      = Color4(10, 10, 14, 140).toNorm(); // Darker modal dim, less washed-out
            // clang-format on

            // Style adjustments — slightly airier spacing, more pronounced rounding
            // for a softer "modern flat" feel; mixed rounding sizes instead of one
            // uniform scale so big surfaces (windows) read as distinctly softer than
            // small controls (frames/tabs).
            style->WindowPadding = glm::vec2(10.00f, 10.00f);
            style->FramePadding = glm::vec2(6.00f, 5.00f);
            style->CellPadding = glm::vec2(8.00f, 6.00f);
            style->ItemSpacing = glm::vec2(8.00f, 6.00f);
            style->ItemInnerSpacing = glm::vec2(6.00f, 6.00f);
            style->TouchExtraPadding = glm::vec2(4.00f, 4.00f);
            style->IndentSpacing = 22;
            style->ScrollbarSize = 12;
            style->GrabMinSize = 12;
            style->WindowBorderSize = 1;
            style->ChildBorderSize = 1;
            style->PopupBorderSize = 1;
            style->FrameBorderSize
                = 0; // borderless frames read as more modern; color contrast carries the shape
            style->TabBorderSize = 0;
            style->LogSliderDeadzone = 4;

            constexpr float windowRounding = 12.f;
            constexpr float controlRounding = 8.f;
            style->WindowRounding = windowRounding;
            style->ChildRounding = controlRounding;
            style->PopupRounding = controlRounding;
            style->FrameRounding = controlRounding;
            style->ScrollbarRounding = windowRounding;
            style->GrabRounding = controlRounding;
            style->TabRounding = controlRounding;
            style->TreeLinesRounding = controlRounding;
        }
    }

    bool GameEditor::needToDraw()
    {
        return !_windows.empty();
    }

    void GameEditor::setupShortcuts()
    {
        _subscriptionPool << keyboardInput.getOrCreate("Close editor", Keyboard::Key::F12)
                                 ->onPress->subscribeAndGetID([&](auto) { GetWindow().close(); });
        auto saveKey = keyboardInput.getOrCreate("Save [Ctrl]", Keyboard::Key::S);
        saveKey->setIsRepeatable(false);
        _subscriptionPool << saveKey->onPress->subscribeAndGetID(
            [&](const KeyboardIA::SpecKeysState& spec)
            {
                if (spec.leftCtrl == Keyboard::KeyState::Pressed
                    && spec.leftShift == Keyboard::KeyState::Pressed)
                {
                    gGameInstance->saveAllToCache();
                }
            });

        auto toggleRenderMode = keyboardInput.getOrCreate("Toggle render mode", Keyboard::Key::F1);
        toggleRenderMode->setIsRepeatable(false);
        _subscriptionPool << toggleRenderMode->onPress->subscribeAndGetID(
            [](auto) { gGameInstance->toggleRenderMode(); });

        _subscriptionPool << keyboardInput.getOrCreate("Cancel action", Keyboard::Key::Escape)
                                 ->onPress->subscribeAndGetID(
                                     [&](auto)
                                     { GetObjectSelectorManager()->deselectAllAndClear(); });

        auto mouseMove = mouseInput.getOrCreate("mouseMove", Mouse::Key::Right);
        _subscriptionPool << mouseMove->onDrag->subscribeAndGetID(
            [this](auto delta, auto spec) { handleMouseDrag(delta, spec); });

        auto selectObject = mouseInput.getOrCreate("selectObject", Mouse::Key::Left);
        selectObject->setIsRepeatable(false);
        _subscriptionPool << selectObject->onMouseClick->subscribeAndGetID(
            [this](auto pos, auto spec) { handleMouseClick(pos, spec); });
    }

    void GameEditor::handleMouseClick(glm::vec2 pos, MouseInputAction::SpecKeysState state)
    {
        if (gDragDrop.getState() == DragAndDrop::State::Dragging)
        {
            return;
        }

        if (auto* objectPicker = GetObjectPicker())
        {
            objectPicker->requestPick([this](Transformable* object) { responseOnPick(object); });
        }
    }

    void GameEditor::responseOnPick(Transformable* object)
    {
        if (!object)
        {
            return;
        }

        if (const auto* wnd = GetEditor()->getWindow<GameViewportEWC>(); !wnd || !wnd->isHovered())
        {
            return;
        }

#ifdef NEXIUM_ENABLE_3D_MODULE
        if (auto* mesh = dynamic_cast<StaticMesh*>(object))
        {
            if (auto* bundle = mesh->tryToGetRootBundle())
            {
                if (!bundle->isIgnoreSelect())
                {
                    GetObjectSelectorManager()->selectSingleObject(bundle);
                }
                bundle->onMousePicked(mesh);
            }
            else
            {
                GetObjectSelectorManager()->selectSingleObject(mesh);
            }
        }
        else
#endif
            if (auto* comp = dynamic_cast<BaseComponent*>(object))
        {
            GetObjectSelectorManager()->selectSingleObject(comp);
        }
    }

    void GameEditor::lazyOneShotInitialization()
    {
        if (auto* dock = getWindow<RootDockWindowEWC>(); Verify(dock))
        {
            for (auto&& winName : dock->getCurrentWindowsList())
            {
                if (auto* win = getWindow<BaseEWC>(winName.c_str()); Verify(win))
                {
                    win->initialize();
                }
            }
        }
    }

    void GameEditor::handleMouseDrag(glm::vec2 delta, MouseInputAction::SpecKeysState state)
    {
    }
} // namespace NX
