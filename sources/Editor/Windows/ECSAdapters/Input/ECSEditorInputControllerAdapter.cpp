// // Nexium
// // Copyright 2018-2026 Valerii Koniushenko
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     http://www.apache.org/licenses/LICENSE-2.0

// #include "ECSEditorInputControllerAdapter.h"

// #include "Editor/GuiComponents/Misc.h"
// #include "Editor/IconsFontAwesome.h"
// #include "ImGui/imgui.h"
// #include "NxSubsystems/Input/InputTypes.h"
// #include "NxWorld/Framework/InputController.h"

// #include <algorithm>
// #include <array>
// #include <optional>
// #include <string>

// namespace
// {
//     template<std::size_t Size>
//     void CopyToBuffer(std::array<char, Size>& destination, const Core::StringAtom& source)
//     {
//         destination.fill('\0');
//         const auto& text = source.toStdString();
//         std::memcpy(destination.data(), text.data(), std::min(text.size(), Size - 1));
//     }

//     NX::InputButton NormalizeModifier(NX::InputButton key)
//     {
//         using Key = Platform::Keyboard::Key;
//         if (key == static_cast<int>(Key::Right_Shift))
//         {
//             return static_cast<int>(Key::Left_Shift);
//         }
//         if (key == static_cast<int>(Key::Right_Control))
//         {
//             return static_cast<int>(Key::Left_Control);
//         }
//         if (key == static_cast<int>(Key::Right_Alt))
//         {
//             return static_cast<int>(Key::Left_Alt);
//         }
//         if (key == static_cast<int>(Key::Right_Super))
//         {
//             return static_cast<int>(Key::Left_Super);
//         }
//         return key;
//     }

//     bool IsModifier(Platform::Keyboard::Key key)
//     {
//         using Key = Platform::Keyboard::Key;
//         return key == Key::Left_Control || key == Key::Left_Shift || key == Key::Left_Alt
//                || key == Key::Left_Super;
//     }

//     std::string_view KeyText(NX::InputButton key)
//     {
//         using Key = Platform::Keyboard::Key;
//         if (key == static_cast<int>(Key::Left_Control))
//         {
//             return "Ctrl";
//         }
//         if (key == static_cast<int>(Key::Left_Shift))
//         {
//             return "Shift";
//         }
//         if (key == static_cast<int>(Key::Left_Alt))
//         {
//             return "Alt";
//         }
//         if (key == static_cast<int>(Key::Left_Super))
//         {
//             return "Super";
//         }
//         return R<Platform::Keyboard::Key>::ToString(
//             static_cast<Platform::Keyboard::Key>(key.getIndex()));
//     }

//     std::string ChordText(const NX::KeyChord& chord)
//     {
//         std::string result;
//         const auto append = [&result](std::string_view value)
//         {
//             if (!result.empty())
//             {
//                 result += " + ";
//             }
//             result += value;
//         };
//         // for (const auto key : chord.requiredKeys)
//         // {
//         //     append(KeyText(key));
//         // }
//         // if (chord.triggerKey != static_cast<int>(Platform::Keyboard::Key::None))
//         // {
//         //     append(KeyText(chord.triggerKey));
//         // }
//         // return result.empty() ? "None" : result;

//         return result.empty() ? "None" : result;
//     }
// } // namespace

// namespace NX
// {
//     ECS_IMPL(ECSEditorInputControllerAdapter);

//     bool ECSEditorInputControllerAdapter::canWorkWith(BaseComponent* component) const
//     {
//         return dynamic_cast<InputController*>(component) != nullptr;
//     }

//     Core::StringAtom ECSEditorInputControllerAdapter::getProcessedAssetType() const
//     {
//         return InputController::componentType;
//     }

//     void ECSEditorInputControllerAdapter::onInitialize()
//     {
//         ECSEditorMimeAdapter::onInitialize();
//     }

//     void ECSEditorInputControllerAdapter::onDraw(float)
//     {
//         if (!Gui::CollapsingHeader("Input shortcuts", ImGuiTreeNodeFlags_DefaultOpen))
//         {
//             return;
//         }

//         auto* controller = dynamic_cast<InputController*>(getTargetComponent());
//         if (!controller)
//         {
//             return;
//         }

//         auto bindings = controller->getBindings();
//         bool changed = false;
//         std::optional<std::size_t> bindingToDelete;

//         for (std::size_t i = 0; i < bindings.size(); ++i)
//         {
//             auto& binding = bindings[i];
//             ImGui::PushID(static_cast<int>(i));

//             std::array<char, 128> action{};
//             CopyToBuffer(action, binding.action);
//             if (ImGui::InputText("Action", action.data(), action.size()))
//             {
//                 binding.action = Core::StringAtom{ action.data() };
//                 changed = true;
//             }

//             const auto recording = _recordingBinding && *_recordingBinding == i;
//             if (recording)
//             {
//                 const auto& keys = R<Platform::Keyboard::Key>::ToArrayC();
//                 for (const auto rawKey : keys)
//                 {
//                     const auto key
//                         = NormalizeModifier(InputButton(static_cast<std::uint16_t>(rawKey)));
//                     if (rawKey == Platform::Keyboard::Key::None
//                         || rawKey == Platform::Keyboard::Key::Last
//                         || !Platform::Keyboard::IsKeyPressed(rawKey)
//                         || std::ranges::find(_recordedKeys, rawKey) != _recordedKeys.end())
//                     {
//                         continue;
//                     }
//                     //     _recordedKeys.push_back(rawKey);
//                     //     if (!IsModifier(rawKey))
//                     //     {
//                     //         _recordedChord.triggerKey =
//                     //         InputButton(static_cast<std::uint16_t>(rawKey));
//                     //     }
//                     // }
//                     // _recordedChord.requiredKeys.clear();
//                     // const auto triggerKeyValue =
//                     //
//                     static_cast<Platform::Keyboard::Key>(_recordedChord.triggerKey.getIndex());
//                     // for (const auto key : _recordedKeys)
//                     // {
//                     //     if (key != triggerKeyValue)
//                     //     {
//                     //
//                     _recordedChord.requiredKeys.push_back(InputButton(static_cast<std::uint16_t>(key)));
//                     //     }
//                 }
//             }

//             const auto shortcutText = ChordText(recording ? _recordedChord : binding.chord);
//             ImGui::InputText("Shortcut", const_cast<char*>(shortcutText.c_str()),
//                              shortcutText.size() + 1, ImGuiInputTextFlags_ReadOnly);

//             if (!recording && ImGui::Button("Record"))
//             {
//                 _recordingBinding = i;
//                 _recordedChord = {};
//                 _recordedKeys.clear();
//             }
//             // if (recording)
//             // {
//             //     ImGui::SameLine();
//             //     ImGui::BeginDisabled(_recordedChord.triggerKey
//             //                          == static_cast<int>(Platform::Keyboard::Key::None));
//             //     if (ImGui::Button("Apply"))
//             //     {
//             //         binding.chord = _recordedChord;
//             //         _recordingBinding.reset();
//             //         changed = true;
//             //     }
//             //     ImGui::EndDisabled();
//             //     ImGui::SameLine();
//             //     if (ImGui::Button("Cancel"))
//             //     {
//             //         _recordingBinding.reset();
//             //     }
//             // }

//             const char* triggerName
//                 = binding.trigger == InputState::Repeated
//                       ? "While held"
//                       : (binding.trigger == InputState::Released ? "On release" : "On press");
//             if (ImGui::BeginCombo("Trigger", triggerName))
//             {
//                 if (ImGui::Selectable("While held", binding.trigger == InputState::Repeated))
//                 {
//                     binding.trigger = InputState::Repeated;
//                     changed = true;
//                 }
//                 if (ImGui::Selectable("On press", binding.trigger == InputState::Pressed))
//                 {
//                     binding.trigger = InputState::Pressed;
//                     changed = true;
//                 }
//                 if (ImGui::Selectable("On release", binding.trigger == InputState::Released))
//                 {
//                     binding.trigger = InputState::Released;
//                     changed = true;
//                 }
//                 ImGui::EndCombo();
//             }

//             if (ImGui::Button(ICON_FA_TRASH " Delete shortcut"))
//             {
//                 bindingToDelete = i;
//             }

//             ImGui::Separator();
//             ImGui::PopID();
//         }

//         if (bindingToDelete)
//         {
//             _recordingBinding.reset();
//             bindings.erase(bindings.begin() + static_cast<std::ptrdiff_t>(*bindingToDelete));
//             changed = true;
//         }

//         // if (ImGui::Button(ICON_FA_PLUS " Add shortcut"))
//         // {
//         //     bindings.push_back(
//         //         { .action = "New action"_atom,
//         //           .chord =
//         KeyChord::Exact(static_cast<uint16_t>(Platform::Keyboard::Key::None)),
//         //           .trigger = InputState::Pressed });
//         //     changed = true;
//         // }

//         if (changed)
//         {
//             controller->setBindings(bindings);
//             makeParentDirty();
//         }
//     }
// } // namespace NX
