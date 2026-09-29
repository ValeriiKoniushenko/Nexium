// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Editor/Windows/NxECSBasedEditor.h"
#include "NxSubsystems/Input/InputTypes.h"

#include <optional>
#include <vector>

namespace NX
{
    CLASS();
    class ECSEditorInputControllerAdapter : public ECSEditorMimeAdapter
    {
        ECS_DECL(ECSEditorInputControllerAdapter, NX::ECSEditorMimeAdapter);

    public:
        [[nodiscard]] bool canWorkWith(BaseComponent* component) const override;
        [[nodiscard]] Core::StringAtom getProcessedAssetType() const override;

    protected:
        void onInitialize() override;
        void onDraw(float dt) override;
        void onApplyAssetData(const nlohmann::json&) override {}

    private:
        KeyChord _recordedChord;
        std::optional<std::size_t> _recordingBinding;
        std::vector<Platform::Keyboard::Key> _recordedKeys;
    };
} // namespace NX

#include "ECSEditorInputControllerAdapter.generated.h"
