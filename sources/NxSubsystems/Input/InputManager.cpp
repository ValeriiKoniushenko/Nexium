// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputManager.h"

namespace NX
{

    Core::StringAtom KeyboardInputManger::getCacheHash() const
    {
        return "KeyboardInputManger"_atom;
    }

    Core::StringAtom MouseInputManger::getCacheHash() const
    {
        return "MouseInputManger"_atom;
    }

    /*void KeyboardInputManger::ioFieldsUpdate(DataStream& out)
    {
        Assert(false, "Not implemented");

        auto stream = out.dedicatedNesting("KeyboardInputManger");


        json["mapping"] = nlohmann::json::array();
        for (const auto& [name, key] : _mapping)
        {
            nlohmann::json map;
            map["action"] = name.toStdString();
            map["key"] = Keyboard::KeyToString(key->getKey().value_or(Keyboard::Key_None));

            json["mapping"].push_back(std::move(map));
        }

    }*/

    /*void MouseInputManger::ioFieldsUpdate(DataStream& out)
    {
        auto stream = out.dedicatedNesting("MouseInputManger");
    }*/
} // namespace NX