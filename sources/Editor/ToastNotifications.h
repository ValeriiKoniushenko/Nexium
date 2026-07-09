/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Core/Color.h"
#include "Core/String.h"
#include "Graphics/Texture.h"

#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace Editor
{
    class NotificationPopUp
    {
    public:
        enum class Type : std::uint8_t
        {
            Custom,
            Info,
            Success,
            Warning,
            Error
        };

        using Timeout = std::chrono::milliseconds;

        NotificationPopUp() = default;
        explicit NotificationPopUp(std::string text);

        [[nodiscard]] static NotificationPopUp Info(std::string text);
        [[nodiscard]] static NotificationPopUp Success(std::string text);
        [[nodiscard]] static NotificationPopUp Warning(std::string text);
        [[nodiscard]] static NotificationPopUp Error(std::string text);

        void show() const;

        void setText(std::string text) { _text = std::move(text); }
        [[nodiscard]] const std::string& getText() const noexcept { return _text; }

        void setTitle(std::string title) { _title = std::move(title); }
        [[nodiscard]] const std::string& getTitle() const noexcept { return _title; }

        void setBorderColor(const Core::Color4 color) noexcept { _borderColor = color; }
        [[nodiscard]] Core::Color4 getBorderColor() const noexcept { return _borderColor; }

        void setBackgroundColor(const Core::Color4 color) noexcept { _backgroundColor = color; }
        [[nodiscard]] Core::Color4 getBackgroundColor() const noexcept { return _backgroundColor; }

        void setImage(Core::StringAtom image) { setImagePath(std::move(image)); }
        [[nodiscard]] const Core::StringAtom& getImage() const noexcept { return _image; }

        void setImagePath(Core::StringAtom image) { _image = std::move(image); }
        [[nodiscard]] const Core::StringAtom& getImagePath() const noexcept { return _image; }

        void setTimeout(const Timeout timeout) noexcept { _timeout = timeout; }
        void setTimeout(const int milliseconds) noexcept { _timeout = Timeout(milliseconds); }
        [[nodiscard]] Timeout getTimeout() const noexcept { return _timeout; }

        void setClosable(const bool value) noexcept { _isClosable = value; }
        [[nodiscard]] bool isClosable() const noexcept { return _isClosable; }

        [[nodiscard]] Type getType() const noexcept { return _type; }

    private:
        friend class NotificationPopUpManager;

        Type _type = Type::Custom;
        std::string _title = "Notification";
        std::string _text;
        Core::StringAtom _image;
        Core::Color4 _borderColor = Core::Color4(117, 142, 168, 255);
        Core::Color4 _backgroundColor = Core::Color4(35, 38, 42, 238);
        Timeout _timeout = Timeout(0);
        bool _isClosable = true;
    };

    class NotificationPopUpManager
    {
    public:
        void show(const NotificationPopUp& notification);
        void tick(float delta);

    private:
        struct ActiveNotification
        {
            NotificationPopUp notification;
            std::optional<Core::Texture> loadedImage;
            float elapsed = 0.f;
            std::uint64_t id = 0;
            bool isClosed = false;
        };

        static void tryLoadImage(ActiveNotification& notification);
        float drawNotification(ActiveNotification& notification, float yOffset, bool& shouldClose);
        void removeExpired();

    private:
        std::vector<ActiveNotification> _notifications;
        std::uint64_t _nextId = 1;
    };
} // namespace Editor
