/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "BaseWindow.h"

#include <set>

namespace Core
{

    class NxEditorBaseEditorEWC : public BaseFloatEWC
    {
        ECS_COMPONENT_DECL(NxEditorBaseEditorEWC, BaseFloatEWC);

    public:
        [[nodiscard]] const char* getIcon() override { return ICON_FA_COG; }

        [[nodiscard]] std::filesystem::path getAssetFilePath() const noexcept;

        void save();
        void openFromFileSystem();
        void openFromPath(const std::filesystem::path& path);
        void discardChanges();

        void putArguments(const StringAtom& args) override;

        void appendFileFilter(const std::string& filter);
        void removeFileFilter(const std::string& filter);
        void clearFileFilters();
        [[nodiscard]] const std::set<std::string>& getFileFilters() const noexcept;

    protected:
        virtual void updateGuiBasedOnAsset() = 0;
        virtual void onSave() = 0;
        virtual void onDiscardChanges() = 0;
        virtual void onOpenFromPath(const std::filesystem::path& path) = 0;
        virtual void onFinishOpenFromPath(const std::filesystem::path& path) {}

        void onInitialize() override;
        void onDraw() final;
        virtual void onDrawProperties() {}
        virtual void onDrawPreview() {}
        void drawMenuBar();
        virtual void onDrawBarExtraFileMenu() {}
        void makeDirty();

    protected:
        std::filesystem::path _assetFilePath;
        std::set<std::string> _fileFilters;
        float _defaultPropertiesWidth = 340.0f;

    private:
        bool _isDirty = false;
        bool _enablePreview = false;
    };

} // namespace Core