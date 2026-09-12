// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseWindow.h"

namespace Core
{

    class SceneObject;
    class Scene;
    class Actor;

    CLASS();
    class SceneTreeWindowEWC : public BaseFloatEWC
    {
        ECS_DECL_NO_CNSTR(SceneTreeWindowEWC, Core::BaseFloatEWC);

    public:
        SceneTreeWindowEWC(const StringAtom& name = ""_atom);

        void setScene(Scene* scene) { _scene = scene; }
        [[nodiscard]] Scene* getScene() const noexcept { return _scene; }

        void highlightSpecificObject(const SceneObject* obj);

        [[nodiscard]] const char* getIcon() override;

    public:
        BaseComponent* selectedObject = nullptr;

    protected:
        void onInitialize() override;
        void onDraw() override;
        void onUpdate() override;

    private:
        void drawTreeNode(BaseComponent* n, int32_t id, bool isInSelectedSubtree = false);
        void processAddNewComponentButton();

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;

        Scene* _scene = nullptr;
        int _commonTreeFlags = 0;
        BaseComponent* _lastSelectedObject = nullptr;

    private:
        const SceneObject* _highlightTracerObject = nullptr;
    };

} // namespace Core

#include "SceneTreeWindow.generated.h" // added by the code generator. Better don't move it.
