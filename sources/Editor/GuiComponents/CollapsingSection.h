// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Editor

#pragma once

#include "VerticalLayout.h"
#include "Widget.h"

#include <optional>
#include <utility>

namespace NX::Gui
{
    /// A collapsing header with rows under it, as a widget (the retained-tree counterpart of
    /// `Gui::CollapsingHeader`). Rows go into `getContent()` and are drawn only while open.
    ///
    /// The header's ImGui id does not depend on the title, so renaming keeps the open state;
    /// with a stable id (`setStableId`) a rebuilt section keeps it too.
    CLASS();
    class CollapsingSection : public Widget
    {
        ECS_DECL_NO_CNSTR(CollapsingSection, NX::Gui::Widget);

    public:
        explicit CollapsingSection(const StringAtom& title = ""_atom);
        /// Clones the rows too (`BaseComponent` clones children) and rebinds to the copy.
        CollapsingSection(const CollapsingSection& other);
        CollapsingSection(CollapsingSection&&) = delete;
        CollapsingSection& operator=(const CollapsingSection&) = delete;
        CollapsingSection& operator=(CollapsingSection&&) = delete;
        ~CollapsingSection() override = default;

        void setTitle(StringAtom title) { _title = std::move(title); }
        [[nodiscard]] const StringAtom& getTitle() const noexcept { return _title; }

        void setHeaderFlags(ImGuiTreeNodeFlags flags) noexcept { _headerFlags = flags; }
        [[nodiscard]] ImGuiTreeNodeFlags getHeaderFlags() const noexcept { return _headerFlags; }

        /// Opens or closes the section on the next draw.
        void setOpen(bool open) noexcept { _requestedOpen = open; }

        /// The state after the last draw; before the first one, the state it will open with.
        [[nodiscard]] bool isOpen() const noexcept;

        [[nodiscard]] VerticalLayout& getContent() noexcept { return *_content; }
        [[nodiscard]] const VerticalLayout& getContent() const noexcept { return *_content; }

        template<IsWidget T, class... Args>
        T* addRow(Args&&... args)
        {
            return _content->addChildComponent<T>(std::forward<Args>(args)...);
        }

        [[nodiscard]] float getWidth() const override;
        [[nodiscard]] float getHeight() const override;
        void setWidth(float newWidth) override { _width = newWidth; }
        void setHeight(float newHeight) override;

    protected:
        void onDraw() override;
        void onAddChild(BaseComponent* newChild) override;

        [[nodiscard]] bool hasVisibleRows() const;
        [[nodiscard]] static float headerHeight();

    protected:
        FIELD();
        Core::StringAtom _title;
        FIELD();
        int _headerFlags = ImGuiTreeNodeFlags_DefaultOpen;

        std::optional<float> _width;
        std::optional<bool> _requestedOpen;
        std::optional<bool> _isOpen;
        VerticalLayout* _content = nullptr;
    };
} // namespace NX::Gui

#include "CollapsingSection.generated.h" // added by the code generator. Better don't move it.
