// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// The editors PropertyGrid uses for engine types. Every pull copies the model into the widget
// only when they differ, so a pull never disturbs a widget that is being edited.

#include "CheckBox.h"
#include "Input.h"
#include "PropertyGrid.h"
#include "VecInput.h"

#include <filesystem>
#include <limits>
#include <string>
#include <tuple>
#include <utility>
#include <variant>

namespace NX::Gui
{
    namespace
    {
        template<class DelegatePtr, class F>
        void Listen(DelegatePtr& delegate, F&& callback)
        {
            std::ignore = delegate->subscribeAndGetID(std::forward<F>(callback));
        }

        /// A JRM limit (`R::Min = 0` or `R::Min = 0.0`) as `T`, clamped to `T`'s range.
        template<IsScalarNumber T>
        [[nodiscard]] T NumberAs(const JRM::Number& number)
        {
            return std::visit(
                [](auto value) -> T
                {
                    using V = decltype(value);
                    constexpr T lowest = std::numeric_limits<T>::lowest();
                    constexpr T highest = std::numeric_limits<T>::max();
                    if constexpr (std::is_floating_point_v<T>)
                    {
                        return static_cast<T>(value);
                    }
                    else if constexpr (std::is_integral_v<V>)
                    {
                        if (std::cmp_less(value, lowest))
                        {
                            return lowest;
                        }
                        if (std::cmp_greater(value, highest))
                        {
                            return highest;
                        }
                        return static_cast<T>(value);
                    }
                    else
                    {
                        if (value <= static_cast<V>(lowest))
                        {
                            return lowest;
                        }
                        if (value >= static_cast<V>(highest))
                        {
                            return highest;
                        }
                        return static_cast<T>(value);
                    }
                },
                number);
        }

        template<class Input>
        void ApplyLimits(Input& input, const RClassField* field)
        {
            using T = typename Input::ValueType;
            if (!field)
            {
                return;
            }
            const auto& attributes = field->attributes;
            if (attributes.min)
            {
                input.setMin(NumberAs<T>(*attributes.min));
            }
            if (attributes.max)
            {
                input.setMax(NumberAs<T>(*attributes.max));
            }
            if (attributes.step)
            {
                input.setStep(NumberAs<T>(*attributes.step));
            }
        }

        template<IsScalarNumber T>
        void NumberEditor(PropertyEditorContext& context)
        {
            auto* input = context.row.addChildComponent<NumInput<T>>();
            input->setFlex(Flex::FlexWidth);
            ApplyLimits(*input, context.field);

            Listen(input->onInput, [binding = context.binding](T value) { binding.set(value); });
            context.row.setPull(
                [input, binding = context.binding]
                {
                    if (const T* value = binding.get<T>(); value && *value != input->getInputtedData())
                    {
                        input->setInputtedData(*value);
                    }
                });
        }

        template<glm::length_t N, IsScalarNumber T>
        void VectorEditor(PropertyEditorContext& context)
        {
            using Vec = glm::vec<N, T>;
            auto* input = context.row.addChildComponent<VecNumInput<N, T>>();
            input->setFlex(Flex::FlexWidth);
            for (auto* component : input->inputs)
            {
                ApplyLimits(*component, context.field);
            }

            Listen(input->onInput, [binding = context.binding](Vec value) { binding.set(value); });
            context.row.setPull(
                [input, binding = context.binding]
                {
                    if (const Vec* value = binding.get<Vec>();
                        value && *value != input->getInputtedData())
                    {
                        input->setInputtedData(*value);
                    }
                });
        }

        void BoolEditor(PropertyEditorContext& context)
        {
            auto* input = context.row.addChildComponent<CheckBox>();
            Listen(input->onChange, [binding = context.binding](bool value) { binding.set(value); });
            context.row.setPull(
                [input, binding = context.binding]
                {
                    if (const bool* value = binding.get<bool>(); value && *value != input->getValue())
                    {
                        input->setValue(*value);
                    }
                });
        }

        template<class Text>
        void TextEditor(PropertyEditorContext& context)
        {
            auto* input = context.row.addChildComponent<TextInput>();
            input->setFlex(Flex::FlexWidth);
            Listen(input->onInput,
                   [binding = context.binding](const char* text)
                   { binding.set(Text(std::string_view(text))); });
            context.row.setPull(
                [input, binding = context.binding]
                {
                    const Text* value = binding.get<Text>();
                    if (!value)
                    {
                        return;
                    }
                    const std::string_view text(*value);
                    if (input->getInputtedData() != text)
                    {
                        input->setInputtedData(std::string(text));
                    }
                });
        }

        void PathEditor(PropertyEditorContext& context)
        {
            using Path = std::filesystem::path;
            auto* input = context.row.addChildComponent<PathInput>();
            input->setFlex(Flex::FlexWidth);
            Listen(input->onPathInput,
                   [binding = context.binding](const Path& path) { binding.set(path); });
            context.row.setPull(
                [input, binding = context.binding]
                {
                    if (const Path* value = binding.get<Path>(); value && *value != input->getPath())
                    {
                        input->setPath(*value);
                    }
                });
        }

        void NormColor3Editor(PropertyEditorContext& context)
        {
            auto* input = context.row.addChildComponent<Color3Input>();
            input->setFlex(Flex::FlexWidth);
            Listen(input->onInput, [binding = context.binding](const Core::Color3& color)
                   { binding.set(color.toNorm()); });
            context.row.setPull(
                [input, binding = context.binding]
                {
                    if (const auto* value = binding.get<Core::NormColor3>();
                        value && value->toColor() != input->getInputtedData())
                    {
                        input->setInputtedData(*value);
                    }
                });
        }

        template<class Color>
        void Color4Editor(PropertyEditorContext& context)
        {
            auto* input = context.row.addChildComponent<Color4Input>();
            input->setFlex(Flex::FlexWidth);
            Listen(input->onInput,
                   [binding = context.binding](const Core::Color4& color)
                   {
                       if constexpr (std::is_same_v<Color, Core::NormColor4>)
                       {
                           binding.set(color.toNorm());
                       }
                       else
                       {
                           binding.set(color);
                       }
                   });
            context.row.setPull(
                [input, binding = context.binding]
                {
                    const Color* value = binding.get<Color>();
                    if (!value)
                    {
                        return;
                    }
                    const Core::Color4 color = [value]
                    {
                        if constexpr (std::is_same_v<Color, Core::NormColor4>)
                        {
                            return value->toColor();
                        }
                        else
                        {
                            return *value;
                        }
                    }();
                    if (color != input->getInputtedData())
                    {
                        input->setInputtedData(color);
                    }
                });
        }

        template<class... T>
        void AddNumbers(PropertyEditors& editors)
        {
            (editors.add<T>("Number"_atom, &NumberEditor<T>), ...);
        }

        template<IsScalarNumber T>
        void AddVectors(PropertyEditors& editors)
        {
            editors.add<glm::vec<2, T>>("Vec2"_atom, &VectorEditor<2, T>);
            editors.add<glm::vec<3, T>>("Vec3"_atom, &VectorEditor<3, T>);
            editors.add<glm::vec<4, T>>("Vec4"_atom, &VectorEditor<4, T>);
        }

        PropertyEditors MakeBuiltin()
        {
            PropertyEditors editors;
            AddNumbers<signed char, unsigned char, short, unsigned short, int, unsigned int, long,
                       unsigned long, long long, unsigned long long, float, double>(editors);
            AddVectors<float>(editors);
            AddVectors<int>(editors);
            AddVectors<double>(editors);
            editors.add<bool>("Bool"_atom, &BoolEditor);
            editors.add<Core::StringAtom>("Text"_atom, &TextEditor<Core::StringAtom>);
            editors.add<std::string>("Text"_atom, &TextEditor<std::string>);
            editors.add<std::filesystem::path>("Path"_atom, &PathEditor);
            editors.add<Core::NormColor3>("Color3"_atom, &NormColor3Editor);
            editors.add<Core::Color4>("Color4"_atom, &Color4Editor<Core::Color4>);
            editors.add<Core::NormColor4>("Color4"_atom, &Color4Editor<Core::NormColor4>);
            return editors;
        }
    } // namespace

    const PropertyEditors& PropertyEditors::Builtin()
    {
        static const PropertyEditors editors = MakeBuiltin();
        return editors;
    }
} // namespace NX::Gui
