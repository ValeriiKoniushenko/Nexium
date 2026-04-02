

/*
 * MIT License
 *
 * Copyright (c) 2018-2026 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "ECS/BaseComponent.h"

class DummyComponent : public Core::BaseComponent
{
    ECS_COMPONENT_DECL(DummyComponent, Core::BaseComponent);

    int a = 123;
    std::string name = "Lola";
};

class HardConstructorComponent : public Core::BaseComponent
{
    ECS_COMPONENT_DECL(HardConstructorComponent, BaseComponent);

    HardConstructorComponent(int a, const Core::StringAtom& name, std::string b)
        : BaseComponent(componentType, name),
          _a(a),
          _b(std::move(b)) {};

    int _a = 0;
    std::string _b;
};

class InitSpyComponent : public Core::BaseComponent
{
    ECS_COMPONENT_DECL(InitSpyComponent, BaseComponent);

public:
    int preInitCalls = 0;
    int initCalls = 0;

protected:
    void onPreInitialize() override { ++preInitCalls; }
    void onInitialize() override { ++initCalls; }
};
