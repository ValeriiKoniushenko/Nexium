// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Editor/Windows/BaseWindow.h"

#include <set>

namespace Core
{

    CLASS();
    class NxEditorBaseEditorEWC : public BaseFloatEWC
    {
        ECS_DECL(NxEditorBaseEditorEWC, Core::BaseFloatEWC);

    public:
        [[nodiscard]] const char* getIcon() override;

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

        void makeDirty();
        [[nodiscard]] bool isDirty() const noexcept { return _isDirty; }

    protected:
        virtual void updateGuiBasedOnAsset() = 0;
        virtual void onSave() = 0;
        virtual void onDiscardChanges() = 0;
        virtual bool onOpenFromPath(const std::filesystem::path& path) = 0;
        virtual void onFinishOpenFromPath(const std::filesystem::path& path) {}

        void onInitialize() override;
        void onDraw() final;
        virtual void onDrawTree() {}
        void treeContextMenuBehavior();
        virtual void onTreeContextMenuBehavior() {}
        virtual void onDrawProperties() {}
        virtual void onDrawPreview() {}
        void drawMenuBar();
        virtual void onDrawBarExtraFileMenu() {}
        void setEnablePreview(bool enable);
        bool isEnablePreview() const noexcept { return _enablePreview; }
        void setEnableTree(bool enable);
        bool isEnableTree() const noexcept { return _enableTree; }

    protected:
        std::filesystem::path _assetFilePath;
        std::set<std::string> _fileFilters;
        constexpr static float _defaultPropertiesWidth = 500.0f;

    private:
        float _lastPropertiesTreeWidth = -1.f;
        bool _isDirty = false;
        bool _enablePreview = false;
        bool _enableTree = false;
    };

} // namespace Core

#include "NxEditorBaseEditor.generated.h" // added by the code generator. Better don't move it.
