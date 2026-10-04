#include "InputBindingsEditor.h"

#include "Editor/Windows/NxECSBasedEditor.h"
#include "ImGui/imgui.h"

#include <algorithm>

namespace NX
{
    ECS_IMPL(InputBindingsEditor);

    void InputBindingsEditor::onInitialize()
    {
        BaseFloatEWC::onInitialize();
        setComponentName("Input bindings"_atom);

        _minWindowSize = FSize2(900.f, 500.f);

        _selectionSubscription = _bindingsList.onSelect->subscribeAndGetID(
            [this](std::size_t index) { selectBinding(index); });
        _addBindingSubscription
            = _bindingsList.onAddBinding->subscribeAndGetID([this] { addBinding(); });

        _bindingsList.initialize();
    }

    void InputBindingsEditor::onDraw()
    {
        validateTarget();
        _bindingsList.draw();
        ImGui::SameLine();

        ImGui::PushID(_targetController.get());
        ImGui::PushID(static_cast<int>(_bindingSession));
        ImGui::PushID(static_cast<int>(_selectedBinding.value_or(_bindings.size())));
        auto* binding = _selectedBinding ? &_bindings.at(*_selectedBinding) : nullptr;
        const auto result = _bindingSettings.draw(binding);
        ImGui::PopID();
        ImGui::PopID();
        ImGui::PopID();
        if (result.deleteRequested)
        {
            deleteSelectedBinding();
        }
        else if (result.changed)
        {
            refreshBindingsList();
            applyBindings();
        }
    }

    void InputBindingsEditor::onClose()
    {
        setTarget(nullptr);
    }

    void InputBindingsEditor::setBindings(const std::vector<InputController::Binding>& bindings)
    {
        _bindings = bindings;
        ++_bindingSession;
        _selectedBinding = _bindings.empty() ? std::nullopt : std::optional<std::size_t>(0);
        _bindingSettings.cancelRecording();
        refreshBindingsList();
    }

    void InputBindingsEditor::setTarget(InputController* controller, NxECSBasedEditorEWC* owner)
    {
        _targetController = controller;
        _owner = owner;
        _hasTarget = controller != nullptr;
        _hasOwner = owner != nullptr;
        if (controller)
        {
            setBindings(controller->getBindings());
        }
        else
        {
            setBindings({});
        }
    }

    void InputBindingsEditor::selectBinding(std::size_t index)
    {
        _bindingSettings.cancelRecording();
        if (index >= _bindings.size())
        {
            _selectedBinding.reset();
            return;
        }

        _selectedBinding = index;
        _bindingsList.setCurrentIndex(index);
    }

    void InputBindingsEditor::refreshBindingsList()
    {
        std::vector<StringAtom> names;
        names.reserve(_bindings.size());
        for (const auto& binding : _bindings)
        {
            names.push_back(binding.action.isEmpty() ? "Unnamed action"_atom : binding.action);
        }
        _bindingsList.setBindings(names);
        if (_selectedBinding)
        {
            _bindingsList.setCurrentIndex(*_selectedBinding);
        }
    }

    void InputBindingsEditor::addBinding()
    {
        auto nextIndex = _bindings.size() + 1;
        auto name = "New binding {}"_f << nextIndex;
        while (std::ranges::any_of(_bindings,
                                   [&name](const auto& binding) { return binding.action == name; }))
        {
            name = "New binding {}"_f << ++nextIndex;
        }

        _bindings.push_back({ .action = name,
                              .chord = KeyChord::Exact(Platform::Keyboard::Key::None),
                              .trigger = InputActionTrigger::OnPress });
        selectBinding(_bindings.size() - 1);
        refreshBindingsList();
        applyBindings();
    }

    void InputBindingsEditor::deleteSelectedBinding()
    {
        if (!_selectedBinding)
        {
            return;
        }

        const auto index = *_selectedBinding;
        _bindings.erase(_bindings.begin() + static_cast<std::ptrdiff_t>(index));
        if (_bindings.empty())
        {
            _selectedBinding.reset();
            _bindingSettings.cancelRecording();
        }
        else
        {
            selectBinding(std::min(index, _bindings.size() - 1));
        }
        refreshBindingsList();
        applyBindings();
    }

    void InputBindingsEditor::applyBindings()
    {
        if (auto controller = _targetController.tryLoad())
        {
            controller->setBindings(_bindings);
            if (auto owner = _owner.tryLoad())
            {
                if (auto* editor = dynamic_cast<NxECSBasedEditorEWC*>(owner.get()))
                {
                    editor->makeDirty();
                }
            }
        }
    }

    void InputBindingsEditor::validateTarget()
    {
        if (!_hasTarget)
        {
            return;
        }

        auto controller = _targetController.tryLoad();
        auto owner = _owner.tryLoad();
        auto* editor = dynamic_cast<NxECSBasedEditorEWC*>(owner.get());
        if (!controller
            || (_hasOwner && (!editor || editor->getTargetComponent() != controller.get())))
        {
            setTarget(nullptr);
        }
    }
} // namespace NX
