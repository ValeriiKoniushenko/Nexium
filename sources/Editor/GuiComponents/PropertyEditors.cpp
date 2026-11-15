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
#include "Core/Position.h"
#include "Core/Size.h"
#include "EnumInputs.h"
#include "Input.h"
#include "InspectorStyle.h"
#include "PropertyCustomization.h"
#include "PropertyGrid.h"
#include "VecInput.h"

#include <array>
#include <filesystem>
#include <format>
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

        // ---- Vector-like engine types (positions, sizes) ----

        template<class Wrapper>
        struct VectorAdapter;

        template<glm::length_t N, class T, glm::qualifier Q>
        struct VectorAdapter<Core::GlobalPosition<N, T, Q>>
        {
            using Vec = glm::vec<N, T, Q>;
            static constexpr bool isSize = false;
            static Vec ToVec(const Core::GlobalPosition<N, T, Q>& value) { return Vec(value); }
            static Core::GlobalPosition<N, T, Q> FromVec(const Vec& value)
            {
                return Core::GlobalPosition<N, T, Q>(value);
            }
        };

        template<glm::length_t N, class T, glm::qualifier Q>
        struct VectorAdapter<Core::RelativePosition<N, T, Q>>
        {
            using Vec = glm::vec<N, T, Q>;
            static constexpr bool isSize = false;
            static Vec ToVec(const Core::RelativePosition<N, T, Q>& value) { return Vec(value); }
            static Core::RelativePosition<N, T, Q> FromVec(const Vec& value)
            {
                return Core::RelativePosition<N, T, Q>(value);
            }
        };

        template<class T, int D>
        struct VectorAdapter<Core::Size<T, D>>
        {
            using Vec = typename Core::Size<T, D>::GlmObjectType;
            static constexpr bool isSize = true;
            static Vec ToVec(const Core::Size<T, D>& value) { return value.toGlm(); }
            static Core::Size<T, D> FromVec(const Vec& value) { return Core::Size<T, D>(value); }
        };

        template<class Wrapper>
        void WrappedVectorEditor(PropertyEditorContext& context)
        {
            using Adapter = VectorAdapter<Wrapper>;
            using Vec = typename Adapter::Vec;
            constexpr auto N = static_cast<std::size_t>(Vec::length());
            using T = typename Vec::value_type;

            auto* input = context.row.addChildComponent<VecNumInput<N, T>>();
            input->setFlex(Flex::FlexWidth);
            if constexpr (Adapter::isSize)
            {
                constexpr std::array<char, 3> names = { 'W', 'H', 'D' };
                std::array<char, N> labels{};
                std::copy_n(names.begin(), N, labels.begin());
                input->setLabelText(labels);
            }
            for (auto* component : input->inputs)
            {
                ApplyLimits(*component, context.field);
            }

            Listen(input->onInput, [binding = context.binding](glm::vec<N, T> value)
                   { binding.set(Adapter::FromVec(Vec(value))); });
            context.row.setPull(
                [input, binding = context.binding]
                {
                    if (const Wrapper* value = binding.get<Wrapper>())
                    {
                        const glm::vec<N, T> vector(Adapter::ToVec(*value));
                        if (vector != input->getInputtedData())
                        {
                            input->setInputtedData(vector);
                        }
                    }
                });
        }

        // ---- Component links: "Name [Type]", read-only ----

        void ComponentLinkEditor(PropertyEditorContext& context)
        {
            using Link = Core::IntrusivePtr<NX::BaseComponent>;
            auto* text = context.row.addChildComponent<TextInput>();
            text->setFlex(Flex::FlexWidth);
            text->setReadOnly(true);
            context.row.setReadOnly(true);
            context.row.setPull(
                [text, binding = context.binding]
                {
                    const Link* link = binding.get<Link>();
                    std::string value = link && link->get()
                                            ? std::format("{} [{}]",
                                                          (*link)->getComponentName().toStdStringView(),
                                                          (*link)->getComponentType().toStdStringView())
                                            : std::string(InspectorStyle::Default().emptyValueText);
                    if (text->getInputtedData() != value)
                    {
                        text->setInputtedData(std::move(value));
                    }
                });
        }

        // ---- AssetRef: a name chosen from PropertyChoices ----

        [[nodiscard]] Core::StringAtom TextOf(const PropertyBinding& binding)
        {
            if (const auto* atom = binding.get<Core::StringAtom>())
            {
                return *atom;
            }
            if (const auto* text = binding.get<std::string>())
            {
                return Core::StringAtom(std::string_view(*text));
            }
            return {};
        }

        [[nodiscard]] Core::StringAtom MetaText(const RClassField* field, std::string_view key)
        {
            const auto value = field ? field->attributes.getMeta(key) : std::nullopt;
            return value ? Core::StringAtom(*value) : Core::StringAtom{};
        }

        void AssetRefEditor(PropertyEditorContext& context)
        {
            const auto& type = context.binding.info().type();
            const bool isAtom = type == typeid(Core::StringAtom);
            if (!isAtom && type != typeid(std::string))
            {
                auto* text = context.row.addChildComponent<TextInput>();
                text->setFlex(Flex::FlexWidth);
                text->setReadOnly(true);
                text->setInputtedData("AssetRef needs a string field");
                context.row.setReadOnly(true);
                return;
            }

            const Core::StringAtom kind = MetaText(context.field, "AssetType");
            const Core::StringAtom dependsOn = MetaText(context.field, "DependsOn");
            std::optional<PropertyBinding> dependency;
            if (!dependsOn.isEmpty() && context.scope && context.scope->sibling)
            {
                dependency = context.scope->sibling(Core::StringAtom::Intern(dependsOn));
            }

            auto* combo = context.row.addChildComponent<ChoiceCombo>();
            combo->setFlex(Flex::FlexWidth);
            combo->setOptionsProvider(
                [choices = context.choices, kind, dependency,
                 owner = context.scope ? context.scope->owner : std::function<void*()>{}]
                {
                    if (!choices)
                    {
                        return std::vector<Core::StringAtom>{};
                    }
                    ChoiceRequest request;
                    request.owner = owner ? owner() : nullptr;
                    if (dependency)
                    {
                        request.dependency = TextOf(*dependency);
                    }
                    return choices->get(kind, request);
                });

            Listen(combo->onSelect,
                   [binding = context.binding, isAtom](const Core::StringAtom& value)
                   {
                       if (isAtom)
                       {
                           binding.set(value);
                       }
                       else
                       {
                           binding.set(value.toStdString());
                       }
                   });
            context.row.setPull(
                [combo, binding = context.binding]
                {
                    const Core::StringAtom value = TextOf(binding);
                    if (combo->getValue() != value)
                    {
                        combo->setValue(value);
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

            editors.add<Core::GlobalPosition2F>("Vec2"_atom, &WrappedVectorEditor<Core::GlobalPosition2F>);
            editors.add<Core::GlobalPosition3F>("Vec3"_atom, &WrappedVectorEditor<Core::GlobalPosition3F>);
            editors.add<Core::RelativePosition2F>("Vec2"_atom,
                                                  &WrappedVectorEditor<Core::RelativePosition2F>);
            editors.add<Core::RelativePosition3F>("Vec3"_atom,
                                                  &WrappedVectorEditor<Core::RelativePosition3F>);
            editors.add<Core::FSize2>("Size2"_atom, &WrappedVectorEditor<Core::FSize2>);
            editors.add<Core::FSize3>("Size3"_atom, &WrappedVectorEditor<Core::FSize3>);
            editors.add<Core::ISize2>("Size2"_atom, &WrappedVectorEditor<Core::ISize2>);

            editors.add<Core::IntrusivePtr<NX::BaseComponent>>("Component"_atom,
                                                               &ComponentLinkEditor);
            editors.addNamed("AssetRef"_atom, &AssetRefEditor);
            return editors;
        }
    } // namespace

    const PropertyEditors& PropertyEditors::Builtin()
    {
        static const PropertyEditors editors = MakeBuiltin();
        return editors;
    }
} // namespace NX::Gui
