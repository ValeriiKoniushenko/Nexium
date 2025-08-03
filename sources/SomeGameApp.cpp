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

#include "Camera/Camera.h"
#include "Core/Timer.h"
#include "Graphics/Image.h"
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Window.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/imgui.h"
#include "InputDevices/InputManager.h"
#include "Misc/FPSCounter.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include <iostream>

using namespace SW;

void setupStyle()
{
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    // Base colors for a pleasant and modern dark theme with dark accents
    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.93f, 0.94f, 1.00f); // Light grey text for readability
    colors[ImGuiCol_TextDisabled]
        = ImVec4(0.50f, 0.52f, 0.54f, 1.00f); // Subtle grey for disabled text
    colors[ImGuiCol_WindowBg]
        = ImVec4(0.14f, 0.14f, 0.16f, 1.00f); // Dark background with a hint of blue
    colors[ImGuiCol_ChildBg]
        = ImVec4(0.16f, 0.16f, 0.18f, 1.00f); // Slightly lighter for child elements
    colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);        // Popup background
    colors[ImGuiCol_Border] = ImVec4(0.28f, 0.29f, 0.30f, 0.60f);         // Soft border color
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);   // No border shadow
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);        // Frame background
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.24f, 0.26f, 1.00f); // Frame hover effect
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.26f, 0.28f, 1.00f);  // Active frame background
    colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);        // Title background
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);  // Active title background
    colors[ImGuiCol_TitleBgCollapsed]
        = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);                          // Collapsed title background
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);   // Menu bar background
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f); // Scrollbar background
    colors[ImGuiCol_ScrollbarGrab]
        = ImVec4(0.24f, 0.26f, 0.28f, 1.00f); // Dark accent for scrollbar grab
    colors[ImGuiCol_ScrollbarGrabHovered]
        = ImVec4(0.28f, 0.30f, 0.32f, 1.00f); // Scrollbar grab hover
    colors[ImGuiCol_ScrollbarGrabActive]
        = ImVec4(0.32f, 0.34f, 0.36f, 1.00f);                               // Scrollbar grab active
    colors[ImGuiCol_CheckMark] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);        // Dark blue checkmark
    colors[ImGuiCol_SliderGrab] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);       // Dark blue slider grab
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f); // Active slider grab
    colors[ImGuiCol_Button] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);           // Dark blue button
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);    // Button hover effect
    colors[ImGuiCol_ButtonActive] = ImVec4(0.32f, 0.42f, 0.52f, 1.00f);     // Active button
    colors[ImGuiCol_Header] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f); // Header color similar to button
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f); // Header hover effect
    colors[ImGuiCol_HeaderActive] = ImVec4(0.32f, 0.42f, 0.52f, 1.00f);  // Active header
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);     // Separator color
    colors[ImGuiCol_SeparatorHovered]
        = ImVec4(0.46f, 0.56f, 0.66f, 1.00f); // Hover effect for separator
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f); // Active separator
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);      // Resize grip
    colors[ImGuiCol_ResizeGripHovered]
        = ImVec4(0.40f, 0.50f, 0.60f, 1.00f); // Hover effect for resize grip
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.44f, 0.54f, 0.64f, 1.00f); // Active resize grip
    colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);              // Inactive tab
    colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);       // Hover effect for tab
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);        // Active tab color
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);     // Unfocused tab
    colors[ImGuiCol_TabUnfocusedActive]
        = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);                        // Active but unfocused tab
    colors[ImGuiCol_PlotLines] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f); // Plot lines
    colors[ImGuiCol_PlotLinesHovered]
        = ImVec4(0.46f, 0.56f, 0.66f, 1.00f); // Hover effect for plot lines
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f); // Histogram color
    colors[ImGuiCol_PlotHistogramHovered]
        = ImVec4(0.40f, 0.50f, 0.60f, 1.00f); // Hover effect for histogram
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f); // Table header background
    colors[ImGuiCol_TableBorderStrong]
        = ImVec4(0.28f, 0.29f, 0.30f, 1.00f); // Strong border for tables
    colors[ImGuiCol_TableBorderLight]
        = ImVec4(0.24f, 0.25f, 0.26f, 1.00f);                            // Light border for tables
    colors[ImGuiCol_TableRowBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);    // Table row background
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.22f, 0.24f, 0.26f, 1.00f); // Alternate row background
    colors[ImGuiCol_TextSelectedBg]
        = ImVec4(0.24f, 0.34f, 0.44f, 0.35f); // Selected text background
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.46f, 0.56f, 0.66f, 0.90f); // Drag and drop target
    colors[ImGuiCol_NavHighlight] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);   // Navigation highlight
    colors[ImGuiCol_NavWindowingHighlight]
        = ImVec4(1.00f, 1.00f, 1.00f, 0.70f); // Windowing highlight
    colors[ImGuiCol_NavWindowingDimBg]
        = ImVec4(0.80f, 0.80f, 0.80f, 0.20f); // Dim background for windowing
    colors[ImGuiCol_ModalWindowDimBg]
        = ImVec4(0.80f, 0.80f, 0.80f, 0.35f); // Dim background for modal windows

    // Style adjustments
    style->WindowPadding = ImVec2(8.00f, 8.00f);
    style->FramePadding = ImVec2(5.00f, 2.00f);
    style->CellPadding = ImVec2(6.00f, 6.00f);
    style->ItemSpacing = ImVec2(6.00f, 6.00f);
    style->ItemInnerSpacing = ImVec2(6.00f, 6.00f);
    style->TouchExtraPadding = ImVec2(0.00f, 0.00f);
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

static void ShowExampleAppMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z"))
            {
            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
            {
            } // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X"))
            {
            }
            if (ImGui::MenuItem("Copy", "CTRL+C"))
            {
            }
            if (ImGui::MenuItem("Paste", "CTRL+V"))
            {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

int main()
{
#ifdef DEBUG
    spdlog::set_level(spdlog::level::trace);
#endif
    std::cout << std::fixed << std::setprecision(15);
    spdlog::set_pattern("%D [%L] [%n] %v");

    //    _    _  _             _
    //   | |  | |(_)           | |
    //   | |  | | _  _ __    __| |  ___ __      __
    //   | |/\| || || '_ \  / _` | / _ \\ \ /\ / /
    //   \  /\  /| || | | || (_| || (_) |\ V  V /
    //    \/  \/ |_||_| |_| \__,_| \___/  \_/\_/
    //----------------------------------------------
    auto& window = GetWindow();
    window.create("Sprite Walker", Core::ISize2{ 1200, 800 });
    // window.toggleCursorMode();

    //    _____  _                 _
    //   /  ___|| |               | |
    //   \ `--. | |__    __ _   __| |  ___  _ __  ___
    //    `--. \| '_ \  / _` | / _` | / _ \| '__|/ __|
    //   /\__/ /| | | || (_| || (_| ||  __/| |   \__ \
    //   \____/ |_| |_| \__,_| \__,_| \___||_|   |___/
    //-------------------------------------------------
    auto& sm = ShaderManager::instance();
    sm.loadShaders("assets/shaders");
    sm.debugLog("Was loaded {} shaders."_f << sm.countOfShaders());
    for (const auto& notLoadedShader : sm.getFailedShaders())
    {
        sm.warnLog("Shader '{}' found but not loaded. It contains some error[s]."_f
                   << notLoadedShader);
    }

    // ======== Setting up[s] ==========
    auto* shader = sm.getShaderProgram("color"_atom);
    Assert(shader);
    shader->setVertexAttributeCallback(
        []()
        {
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                  reinterpret_cast<void*>(3 * sizeof(float)));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                  reinterpret_cast<void*>(6 * sizeof(float)));
        });

    //    _    _               _      _     _   _
    //   | |  | |             | |    | |   | | | |
    //   | |  | |  ___   _ __ | |  __| |   | | | |  __ _  _ __  ___
    //   | |/\| | / _ \ | '__|| | / _` |   | | | | / _` || '__|/ __|
    //   \  /\  /| (_) || |   | || (_| |   \ \_/ /| (_| || |   \__ \
    //    \/  \/  \___/ |_|   |_| \__,_|    \___/  \__,_||_|   |___/
    //---------------------------------------------------------------
    float timeDelta = 0.f;
    glm::vec3 lightPos(1'000'000.f, 1'000'000.f, 1'000'000.f);
    BaseCamera camera;
    camera.moveForward(-100);
    camera.setFov(90.f);
    std::vector<StaticMeshBundle> meshes;

    //    _____                       _        ___         _    _
    //   |_   _|                     | |      / _ \       | |  (_)
    //     | |   _ __   _ __   _   _ | |_    / /_\ \  ___ | |_  _   ___   _ __   ___
    //     | |  | '_ \ | '_ \ | | | || __|   |  _  | / __|| __|| | / _ \ | '_ \ / __|
    //    _| |_ | | | || |_) || |_| || |_    | | | || (__ | |_ | || (_) || | | |\__ \
    //    \___/ |_| |_|| .__/  \__,_| \__|   \_| |_/ \___| \__||_| \___/ |_| |_||___/
    //                 | |
    //                 |_|
    //--------------------------------------------------------------------------------
    KeyboardInputManger keyboardInput;
    MouseInputManger mouseInput;

    // clang-format off
    constexpr float speed = 50.f, mouseSensitivity = 700.0;
    auto getRealSpeed = [speed](KeyboardIA::SpecKeysState state)
    {
        const float mlt = state.leftShift.cast() == Keyboard::KeyState::Pressed ? 10.f : 1.f;
        return speed * mlt;
    };
    keyboardInput.create("moveForward", GLFW_KEY_W)->onPress.subscribe([&](auto state){ camera.moveForward(getRealSpeed(state) * timeDelta); });
    keyboardInput.create("moveBackward", GLFW_KEY_S)->onPress.subscribe([&](auto state){ camera.moveForward(-getRealSpeed(state) * timeDelta); });
    keyboardInput.create("moveRight", GLFW_KEY_D)->onPress.subscribe([&](auto state){ camera.moveRight(-getRealSpeed(state) * timeDelta); });
    keyboardInput.create("moveLeft", GLFW_KEY_A)->onPress.subscribe([&](auto state){ camera.moveRight(getRealSpeed(state) * timeDelta); });
    keyboardInput.create("moveUp", GLFW_KEY_SPACE)->onPress.subscribe([&](auto state){ camera.moveUp(-getRealSpeed(state) * timeDelta); });
    keyboardInput.create("moveDown", GLFW_KEY_C)->onPress.subscribe([&](auto state){ camera.moveUp(getRealSpeed(state) * timeDelta); });
    keyboardInput.create("exit", GLFW_KEY_ESCAPE)->onPress.subscribe([&](auto){ window.close(); });
    auto toggleCursorMode = keyboardInput.create("toggleCursorMode", GLFW_KEY_M);
    toggleCursorMode->onPress.subscribe([&](auto) { window.toggleCursorMode(); });
    toggleCursorMode->setIsRepeatable(false);
    mouseInput.create("cameraView", 0)->onMove.subscribe([&](glm::vec2 delta, auto){ camera.yawAndPitch(delta * timeDelta * mouseSensitivity); });
    // clang-format on

    // ====================== MISC ==========================
    std::vector<std::filesystem::path> modelPaths
        = { /*"assets/base-3d/Models/FBX/Tree.fbx",*/ "assets/base-3d/Models/FBX/FireHydrant.fbx" };

    Assimp::Importer importer;
    for (auto&& path : modelPaths)
    {
        const aiScene* scene = importer.ReadFile(
            path.generic_string().c_str(),
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
        if (Verify(scene) && Verify(scene->mRootNode))
        {
            StaticMeshBundle mesh;
            mesh.importFrom(scene->mRootNode, scene, path);
            mesh.setShaderProgram(shader);
            meshes.push_back(std::move(mesh));
        }
    }

    //   ___  ___        _          _
    //   |  \/  |       (_)        | |
    //   | .  . |  __ _  _  _ __   | |      ___    ___   _ __
    //   | |\/| | / _` || || '_ \  | |     / _ \  / _ \ | '_ \
    //   | |  | || (_| || || | | | | |____| (_) || (_) || |_) |
    //   \_|  |_/ \__,_||_||_| |_| \_____/ \___/  \___/ | .__/
    //                                                  | |
    //                                                  |_|
    //-----------------------------------------------------------
    Core::FStopwatch clock;

    FPSCounter fpsCounter;
    fpsCounter.start();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImFont* myFont = io.Fonts->AddFontFromFileTTF("assets/fonts/JetBrainsMono-Regular.ttf", 16.0f);
    ImGui::PushFont(myFont);

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    setupStyle();
    ImGui_ImplGlfw_InitForOpenGL(GetWindow().getRawWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    auto render = Core::ISize2{ 400, 400 };
    GLuint fbo = 0, tex = 0, rbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create color texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, render.width, render.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    // Create and attach depth buffer (REQUIRED!)
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, render.width, render.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        globalLog.criticalThrowingLog("Framebuffer for editor render is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!window.shouldClose())
    {
        clock.start();
        window.pollEvent();

        keyboardInput.update();
        mouseInput.update();

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, render.width, render.height);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->use();
        shader->setUniform("uObjectColor"_atom, 1.0f, 1.0f, 1.0f);
        shader->setUniform("uLightColor"_atom, 1.0f, 1.0f, 1.0f);
        shader->setUniform("uLightPos"_atom, lightPos);
        shader->setUniform("uViewPos"_atom, camera.getPosition());
        shader->setUniform("uTexture"_atom, 0);
        shader->setUniform("uProjAndView"_atom, camera.getMatrix());

        meshes.front().directDraw();
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind after setup

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("My Window");

        if (ImGui::BeginTabBar("MyTabBar"))
        {
            if (ImGui::BeginTabItem("Tab 1"))
            {
                ImGui::Text("This is Tab 1");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Tab 2"))
            {
                ImGui::Text("This is Tab 2");
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        ShowExampleAppMainMenuBar();
        {
            ImGui::Begin("My Render View");
            ImGui::Image((void*)(intptr_t)tex, ImVec2(400, 400), ImVec2(0.0f, 1.0f),
                         ImVec2(1.0f, 0.0f));
            ImGui::End();
        }
        /*
        {
            ImGui::Begin("Hello, world!");
            ImGui::Text("This is some useful text.");
            static bool a = false;
            static bool b = false;
            static float f = 0;
            static int counter = 0;
            ImGui::Checkbox("Demo Window", &a);
            ImGui::Checkbox("Another Window", &b);
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clear_color));
            if (ImGui::Button("Button"))
            {
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                        io.Framerate);
            ImGui::End();
        }
        */
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default framebuffer

        window.swapBuffers();

        fpsCounter.newFrameUpdate();
        timeDelta = clock.stop();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    globalLog.infoLog("FPS: {}"_f << fpsCounter.getFPS());

    return 0;
}
