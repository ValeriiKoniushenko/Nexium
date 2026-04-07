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

#include "DummyComponent.h"

ECS_COMPONENT_IMPL(DummyComponent);
ECS_R_FRIEND_IMPL(DummyComponent);

ECS_COMPONENT_IMPL(HardConstructorComponent);
ECS_R_FRIEND_IMPL(HardConstructorComponent);

ECS_COMPONENT_IMPL(InitSpyComponent);
ECS_R_FRIEND_IMPL(InitSpyComponent);

ECS_COMPONENT_IMPL(Vehicle);
ECS_R_FRIEND_IMPL(Vehicle);

ECS_COMPONENT_IMPL(BaseCar);
ECS_R_FRIEND_IMPL(BaseCar);

ECS_COMPONENT_IMPL(Sedan);
ECS_R_FRIEND_IMPL(Sedan);

ECS_COMPONENT_IMPL(BasePart);
ECS_R_FRIEND_IMPL(BasePart);

ECS_COMPONENT_IMPL(Engine);
ECS_R_FRIEND_IMPL(Engine);

ECS_COMPONENT_IMPL(TurboEngine);
ECS_R_FRIEND_IMPL(TurboEngine);

ECS_COMPONENT_IMPL(Wheel);
ECS_R_FRIEND_IMPL(Wheel);

ECS_COMPONENT_IMPL(Interior);
ECS_R_FRIEND_IMPL(Interior);

ECS_COMPONENT_IMPL(Seat);
ECS_R_FRIEND_IMPL(Seat);

ECS_COMPONENT_IMPL(Dashboard);
ECS_R_FRIEND_IMPL(Dashboard);

ECS_COMPONENT_IMPL(Electronics);
ECS_R_FRIEND_IMPL(Electronics);

ECS_COMPONENT_IMPL(Sensor);
ECS_R_FRIEND_IMPL(Sensor);

ECS_COMPONENT_IMPL(Camera);
ECS_R_FRIEND_IMPL(Camera);
