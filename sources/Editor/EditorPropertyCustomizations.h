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

namespace NX::Gui
{
    class PropertyChoices;
    class PropertyCustomizations;
    class PropertyEditors;
} // namespace NX::Gui

namespace NX
{
    /// Registers how the engine's classes look in property grids: section titles, computed and
    /// custom rows, asset choice lists ("Atlas", "AtlasFrame", "Scene", "RectangleAnimation") and
    /// editors for engine value types.
    void RegisterEditorPropertyCustomizations(Gui::PropertyCustomizations& customizations,
                                              Gui::PropertyChoices& choices,
                                              Gui::PropertyEditors& editors);

    /// Registers them into the global registries the first time it is called; later calls do
    /// nothing. Every editor window that hosts a PropertyGrid calls it when it opens.
    void EnsureEditorPropertyCustomizations();
} // namespace NX
