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

#include "Core/String.h"
#include "Utils/Concepts.h"

#include <cstddef>

namespace NX::Gui
{
    enum class Align;
    enum class Flex;

    class ArrayCell;
    class BaseCombo;
    class BaseInput;
    class BaseList;
    class Button;
    class CheckBox;
    class Color3Input;
    class ComboModelBased;
    class ComboView;
    class HorizontalLayout;
    class ImageButton;
    class Label;
    class Layout;
    class ListModelBased;
    class ListView;
    class Separator;
    class Spacer;
    class TextInput;
    class ToggleButton;
    class VerticalLayout;
    class Widget;

    template<class T>
    class LabelRow;

    template<class T, class ArrayCellViewerFunc, class ViewFetchFunc>
    class BaseArray;

    template<Utils::IsArithmetic T>
    class NumInput;

    template<std::size_t Size, Utils::IsArithmetic Type>
    class VecNumInput;

    struct _StringArray_ArrayCellViewerFunc;
    struct _StringArray_ViewFetchFunc;

    using DoubleInput = NumInput<double>;
    using FloatInput = NumInput<float>;
    using IntInput = NumInput<int>;
    using StringArray
        = BaseArray<Core::StringAtom, _StringArray_ArrayCellViewerFunc, _StringArray_ViewFetchFunc>;

    using Int4Input = VecNumInput<4, int>;
    using Float4Input = VecNumInput<4, float>;
    using Double4Input = VecNumInput<4, double>;

    using Int3Input = VecNumInput<3, int>;
    using Float3Input = VecNumInput<3, float>;
    using Double3Input = VecNumInput<3, double>;

    using Int2Input = VecNumInput<2, int>;
    using Float2Input = VecNumInput<2, float>;
    using Double2Input = VecNumInput<2, double>;
} // namespace NX::Gui
