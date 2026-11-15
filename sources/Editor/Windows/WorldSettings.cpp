// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "WorldSettings.h"

#include "Editor/EditorPropertyCustomizations.h"
#include "Editor/GuiComponents/InspectorStyle.h"
#include "Editor/GuiComponents/PropertyGrid.h"
#include "Editor/IconsFontAwesome.h"
#include "NxWorld/Framework/GameInstanceAccess.h"
#include "NxWorld/Framework/World.h"

using namespace NX;

namespace NX
{
    namespace
    {
        Gui::PropertyGrid::Ptr MakeGrid(const char* id, Gui::PropertyGrid::TargetResolver target)
        {
            auto grid = Gui::PropertyGrid::Create();
            grid->setStableId(Core::StringAtom::Intern(id));
            grid->setSections(Gui::PropertySections::PerClass);
            grid->setLabelWidth(Gui::InspectorStyle::Default().wideLabelWidth);
            grid->setTarget(std::move(target));
            return grid;
        }
    } // namespace

    WorldSettingsEWC::WorldSettingsEWC(const StringAtom& name)
        : BaseFloatEWC(componentType, name)
    {
        if (name.isEmpty())
        {
            setComponentName("World settings"_atom);
        }
    }

    ECS_IMPL(WorldSettingsEWC);

    const char* WorldSettingsEWC::getIcon()
    {
        return ICON_FA_SUN_O;
    }

    void WorldSettingsEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();
    }

    void WorldSettingsEWC::onOpen()
    {
        BaseFloatEWC::onOpen();
        EnsureEditorPropertyCustomizations();

        Assert(!_worldGrid && !_lightningGrid);
        _worldGrid = MakeGrid("World",
                              []
                              {
                                  auto* world = GetWorld();
                                  return world ? Reflection::ObjectRef(*world)
                                               : Reflection::ObjectRef{};
                              });
        _lightningGrid = MakeGrid("Lightning",
                                  []
                                  {
                                      auto* world = GetWorld();
                                      return world ? Reflection::ObjectRef(world->lightning)
                                                   : Reflection::ObjectRef{};
                                  });
    }

    void WorldSettingsEWC::onClose()
    {
        BaseEWC::onClose();
        _worldGrid = nullptr;
        _lightningGrid = nullptr;
    }

    void WorldSettingsEWC::onDraw()
    {
        const float delta = GetWorld()->getTimeDelta();
        if (_worldGrid)
        {
            _worldGrid->tick(delta);
        }
        if (_lightningGrid)
        {
            _lightningGrid->tick(delta);
        }
    }
} // namespace NX
