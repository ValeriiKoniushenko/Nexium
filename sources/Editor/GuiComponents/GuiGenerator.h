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

#include "Editor/ForwardDeclarations.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/VecInput.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "NxFundamental/ECS/BaseComponent.h"

#include <JustReflectMe/Adapter.h>

namespace NX::Gui
{

    class GuiGenerator : public Gui::VerticalLayout
    {
    public:
        [[nodiscard]] static Core::StringAtom NormalizeLabelName(const Core::StringAtom& name);

        template<Utils::IsArithmetic T>
        void spawnArithmetic(const Core::StringAtom& label, T& value, const RClassField& field);

        template<glm::length_t L, typename T>
        void spawnGlmVec(const Core::StringAtom& label, glm::vec<L, T>& value,
                         const RClassField& field);

        void spawnNormColor3(const Core::StringAtom& label, NormColor3& value,
                             const RClassField& field);

        void despawnEverything();

        void setLabelWidth(float v) noexcept;
        [[nodiscard]] float getLabelWidth() const noexcept { return _labelWidth; }

        void spawnRouter(void* data, const char* type, const Core::StringAtom& label,
                         const RClassField& field);

    protected:
        float _labelWidth = 140.f;
        DelegateSubscriberPoolGuard _subscriptionPool;
    };

    template<class T>
    class ObjectBasedGuiGenerator : public GuiGenerator
    {
    public:
        explicit ObjectBasedGuiGenerator(const std::function<T*()>& getter)
        {
            setObjectGetter(getter);
        }

        ObjectBasedGuiGenerator() = default;
        ObjectBasedGuiGenerator(ObjectBasedGuiGenerator&&) = default;
        ObjectBasedGuiGenerator(const ObjectBasedGuiGenerator&) = default;
        ObjectBasedGuiGenerator& operator=(ObjectBasedGuiGenerator&&) = default;
        ObjectBasedGuiGenerator& operator=(const ObjectBasedGuiGenerator&) = default;
        ~ObjectBasedGuiGenerator() override = default;

        void setObjectGetter(const std::function<T*()>& getter) { _objectGetter = getter; }
        void generateGui()
        {
            auto* obj = _objectGetter();

            if (!obj)
            {
                return;
            }

            const auto fields = R<T>::GetFieldsMap();
            for (auto&& f : fields)
            {
                R<T>::GetField(*obj, f.first.c_str(), [&](void* data, const char* type)
                               { spawnRouter(data, type, f.first.c_str(), f.second); });
            }
        }

    protected:
        std::function<T*()> _objectGetter;
    };

    // =======================================================
    //                  IMPLEMENTATIONS
    // =======================================================

    template<Utils::IsArithmetic T>
    void GuiGenerator::spawnArithmetic(const Core::StringAtom& label, T& value,
                                       const RClassField& field)
    {
        using ValueType = std::conditional_t<std::is_integral_v<T>, int64_t, double>;
        using GuiValType = std::conditional_t<std::is_integral_v<ValueType>, int, float>;

        auto* c = addChildComponent<Gui::LabelRow<Gui::NumInput<GuiValType>>>();
        c->label->setText(NormalizeLabelName(label));
        c->label->setWidth(_labelWidth);
        c->input->setInputtedData(value);
        if (field.attributes.min)
        {
            const auto v = std::get<ValueType>(*field.attributes.min);
            c->input->setMin(static_cast<GuiValType>(v));
        }
        if (field.attributes.max)
        {
            const auto v = std::get<ValueType>(*field.attributes.max);
            c->input->setMax(static_cast<GuiValType>(v));
        }
        if (field.attributes.step)
        {
            const auto v = std::get<ValueType>(*field.attributes.step);
            c->input->setStep(static_cast<GuiValType>(v));
        }
        _subscriptionPool << c->input->onInput->subscribeAndGetID([&value](ValueType in)
                                                                  { value = in; });
    }

    template<glm::length_t L, typename T>
    void GuiGenerator::spawnGlmVec(const Core::StringAtom& label, glm::vec<L, T>& value,
                                   const RClassField& field)
    {
        using ValueType = std::conditional_t<std::is_integral_v<T>, int64_t, double>;
        using GuiValType = std::conditional_t<std::is_integral_v<ValueType>, int, float>;

        auto* c = addChildComponent<Gui::LabelRow<Gui::VecNumInput<L, GuiValType>>>();
        c->label->setText(NormalizeLabelName(label));
        c->label->setWidth(_labelWidth);

        c->input->setInputtedData(value);
        c->input->setFlex(Gui::Flex::Fixed);
        c->input->setWidth(80.f * static_cast<float>(L));

        for (auto* i : c->input->inputs)
        {
            if (field.attributes.min)
            {
                const auto v = std::get<ValueType>(*field.attributes.min);
                i->setMin(static_cast<GuiValType>(v));
            }
            if (field.attributes.max)
            {
                const auto v = std::get<ValueType>(*field.attributes.max);
                i->setMax(static_cast<GuiValType>(v));
            }
            if (field.attributes.step)
            {
                const auto v = std::get<ValueType>(*field.attributes.step);
                i->setStep(static_cast<GuiValType>(v));
            }
        }
        _subscriptionPool << c->input->onInput->subscribeAndGetID([&value](glm::vec<L, T> in)
                                                                  { value = in; });
    }

} // namespace NX::Gui