// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "ToastNotifications.h"

#include "Editor/EditorIntegration.h"
#include "ImGui/imgui.h"
#include "Misc/IconsFontAwesome.h"

#include <algorithm>
#include <utility>

namespace Editor
{
    namespace
    {
        constexpr float NotificationWidth = 320.f;
        constexpr float NotificationMargin = 14.f;
        constexpr float NotificationSpacing = 8.f;

        [[nodiscard]] ImU32 ToImGuiColor(const Core::Color4& color)
        {
            return IM_COL32(color.r, color.g, color.b, color.a);
        }
    } // namespace

    NotificationPopUp::NotificationPopUp(std::string text)
        : _text(std::move(text))
    {
    }

    NotificationPopUp NotificationPopUp::Info(std::string text)
    {
        NotificationPopUp notification(std::move(text));
        notification._type = Type::Info;
        notification._title = ICON_FA_INFO_CIRCLE " Info";
        notification._borderColor = Core::Color4(117, 142, 168, 255);
        notification._backgroundColor = Core::Color4(35, 38, 42, 238);
        notification._timeout = Timeout(3000);
        notification._isClosable = true;
        return notification;
    }

    NotificationPopUp NotificationPopUp::Success(std::string text)
    {
        NotificationPopUp notification(std::move(text));
        notification._type = Type::Success;
        notification._title = ICON_FA_CHECK_CIRCLE " Success";
        notification._borderColor = Core::Color4(80, 168, 116, 255);
        notification._backgroundColor = Core::Color4(35, 38, 42, 238);
        notification._timeout = Timeout(2500);
        notification._isClosable = true;
        return notification;
    }

    NotificationPopUp NotificationPopUp::Warning(std::string text)
    {
        NotificationPopUp notification(std::move(text));
        notification._type = Type::Warning;
        notification._title = ICON_FA_EXCLAMATION_TRIANGLE " Warning";
        notification._borderColor = Core::Color4(219, 172, 75, 255);
        notification._backgroundColor = Core::Color4(45, 39, 30, 245);
        notification._timeout = Timeout(0);
        notification._isClosable = true;
        return notification;
    }

    NotificationPopUp NotificationPopUp::Error(std::string text)
    {
        NotificationPopUp notification(std::move(text));
        notification._type = Type::Error;
        notification._title = ICON_FA_TIMES_CIRCLE " Error";
        notification._borderColor = Core::Color4(219, 91, 91, 255);
        notification._backgroundColor = Core::Color4(48, 32, 34, 245);
        notification._timeout = Timeout(0);
        notification._isClosable = true;
        return notification;
    }

    void NotificationPopUp::show() const
    {
        if (gGameInstance)
        {
            Core::GetEditor()->notifications.show(*this);
        }
    }

    void NotificationPopUpManager::show(const NotificationPopUp& notification)
    {
        ActiveNotification activeNotification;
        activeNotification.notification = notification;
        activeNotification.id = _nextId++;
        tryLoadImage(activeNotification);
        _notifications.emplace_back(std::move(activeNotification));
    }

    void NotificationPopUpManager::tryLoadImage(ActiveNotification& notification)
    {
        const auto& imagePath = notification.notification.getImagePath();
        if (imagePath.isEmpty())
        {
            return;
        }

        notification.loadedImage.emplace();
        if (!notification.loadedImage->loadFromFile(imagePath.c_str(), false))
        {
            notification.loadedImage.reset();
        }
    }

    void NotificationPopUpManager::tick(const float delta)
    {
        float yOffset = NotificationMargin;
        for (auto& notification : _notifications)
        {
            notification.elapsed += delta;

            bool shouldClose = false;
            const float height = drawNotification(notification, yOffset, shouldClose);
            if (shouldClose)
            {
                notification.isClosed = true;
            }

            yOffset += height + NotificationSpacing;
        }

        removeExpired();
    }

    float NotificationPopUpManager::drawNotification(ActiveNotification& notification,
                                                     float yOffset, bool& shouldClose)
    {
        const auto& data = notification.notification;
        float height = 0.f;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const glm::vec2 position
            = { viewport->WorkPos.x + viewport->WorkSize.x - NotificationMargin,
                viewport->WorkPos.y + yOffset };
        ImGui::SetNextWindowPos(position, ImGuiCond_Always, { 1.f, 0.f });
        ImGui::SetNextWindowSize({ NotificationWidth, 0.f }, ImGuiCond_Always);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ToImGuiColor(data.getBackgroundColor()));
        ImGui::PushStyleColor(ImGuiCol_Border, ToImGuiColor(data.getBorderColor()));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 12.f, 10.f });

        const std::string windowName = "NotificationPopUp##" + std::to_string(notification.id);
        constexpr ImGuiWindowFlags flags
            = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking
              | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
              | ImGuiWindowFlags_AlwaysAutoResize;

        if (ImGui::Begin(windowName.c_str(), nullptr, flags))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ToImGuiColor(data.getBorderColor()));
            ImGui::TextUnformatted(data.getTitle().c_str());
            ImGui::PopStyleColor();

            if (data.isClosable())
            {
                ImGui::SameLine(ImGui::GetWindowWidth() - 28.f);
                if (ImGui::SmallButton(ICON_FA_TIMES))
                {
                    shouldClose = true;
                }
            }

            if (notification.loadedImage && notification.loadedImage->isValid())
            {
                constexpr float imageSize = 28.f;
                ImGui::Image(notification.loadedImage->getTextureId(), { imageSize, imageSize });
                ImGui::SameLine();
            }

            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + NotificationWidth - 24.f);
            ImGui::TextUnformatted(data.getText().c_str());
            ImGui::PopTextWrapPos();

            height = ImGui::GetWindowHeight();
        }
        ImGui::End();

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);

        return height;
    }

    void NotificationPopUpManager::removeExpired()
    {
        std::erase_if(_notifications,
                      [](const ActiveNotification& notification)
                      {
                          const auto timeout = notification.notification.getTimeout();
                          return notification.isClosed
                                 || (timeout.count() > 0
                                     && notification.elapsed
                                            >= static_cast<float>(timeout.count()) / 1000.f);
                      });
    }
} // namespace Editor
