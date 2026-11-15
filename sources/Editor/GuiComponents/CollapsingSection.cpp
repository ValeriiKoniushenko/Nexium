// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "CollapsingSection.h"

#include "ImGui/imgui_internal.h"

#include <algorithm>

namespace NX::Gui
{
    ECS_IMPL(CollapsingSection);

    CollapsingSection::CollapsingSection(const StringAtom& title)
        : Widget(componentType, title.isEmpty() ? "CollapsingSection"_atom : title),
          _title(title)
    {
        setFlex(Flex::FlexWidth);
        _content = addChildComponent<VerticalLayout>("Content"_atom);
    }

    CollapsingSection::CollapsingSection(const CollapsingSection& other)
        : Widget(other),
          _title(other._title),
          _headerFlags(other._headerFlags),
          _width(other._width),
          _requestedOpen(other._requestedOpen)
    {
        if (!getChildren().empty())
        {
            _content = getChildren().front()->tryCastTo<VerticalLayout>();
        }
        if (!_content)
        {
            _content = addChildComponent<VerticalLayout>("Content"_atom);
        }
    }

    bool CollapsingSection::isOpen() const noexcept
    {
        return _isOpen.value_or(
            _requestedOpen.value_or((_headerFlags & ImGuiTreeNodeFlags_DefaultOpen) != 0));
    }

    float CollapsingSection::getWidth() const
    {
        if (_width)
        {
            return *_width;
        }
        if (const auto* parent = getParentAs<Widget>())
        {
            return parent->getWidth();
        }
        return ImGui::GetContentRegionAvail().x;
    }

    float CollapsingSection::getHeight() const
    {
        float height = headerHeight();
        if (isOpen() && hasVisibleRows())
        {
            height += style().ItemSpacing.y + _content->getHeight();
        }
        return height;
    }

    void CollapsingSection::setHeight(float /*newHeight*/)
    {
        // The height follows the header and the rows; there is nothing to resize.
    }

    void CollapsingSection::onDraw()
    {
        // Same spacing as Gui::CollapsingHeader, so generated and hand-written sections match.
        ImGui::Dummy(glm::vec2(0.f, style().ItemSpacing.y));

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
        {
            _isOpen = false;
            return;
        }

        // After the Dummy: submitting an item consumes the "next item" data.
        if (_requestedOpen)
        {
            ImGui::SetNextItemOpen(*_requestedOpen, ImGuiCond_Always);
            _requestedOpen.reset();
        }

        // The id is fixed and the title is only displayed, so renaming keeps the state.
        const ImGuiID id = window->GetID("##section");
        _isOpen = ImGui::TreeNodeBehavior(id, _headerFlags | ImGuiTreeNodeFlags_CollapsingHeader,
                                          _title.c_str());

        if (*_isOpen && hasVisibleRows())
        {
            _content->unhandledDraw();
        }
    }

    void CollapsingSection::onAddChild(BaseComponent* newChild)
    {
        Widget::onAddChild(newChild);
        newChild->unsafeCastTo<Widget>()->setIsAutoDraw(false);
    }

    bool CollapsingSection::hasVisibleRows() const
    {
        return _content
               && std::ranges::any_of(_content->getChildren(),
                                      [](const auto& child) { return child->isEnabled(); });
    }

    float CollapsingSection::headerHeight()
    {
        const float spacing = ImGui::GetStyle().ItemSpacing.y;
        return spacing + spacing + ImGui::GetFrameHeight();
    }
} // namespace NX::Gui
