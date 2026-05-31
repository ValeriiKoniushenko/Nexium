

/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

CLASS();
class DummyComponent : public Core::BaseComponent
{
    R_FRIEND_DECL(DummyComponent, Core::BaseComponent);
    ECS_COMPONENT_DECL(DummyComponent, Core::BaseComponent);

    FIELD();
    int a = 123;

    FIELD();
    std::string name = "Lola";
};

CLASS();
class HardConstructorComponent : public Core::BaseComponent
{
    R_FRIEND_DECL(HardConstructorComponent, Core::BaseComponent);
    ECS_COMPONENT_DECL(HardConstructorComponent, BaseComponent);

    HardConstructorComponent(int a, const Core::StringAtom& name, std::string b)
        : BaseComponent(componentType, name),
          _a(a),
          _b(std::move(b))
    {
    }

    int _a = 0;
    std::string _b;
};

CLASS();
class InitSpyComponent : public Core::BaseComponent
{
    R_FRIEND_DECL(InitSpyComponent, Core::BaseComponent);
    ECS_COMPONENT_DECL(InitSpyComponent, BaseComponent);

public:
    int preInitCalls = 0;
    int initCalls = 0;

protected:
    void onPreInitialize() override { ++preInitCalls; }
    void onInitialize() override { ++initCalls; }
};

CLASS();
class Vehicle : public Core::BaseComponent
{
    R_FRIEND_DECL(Vehicle, Core::BaseComponent);
    ECS_COMPONENT_DECL(Vehicle, Core::BaseComponent);

    FIELD();
    std::string vin = "UNKNOWN";
    FIELD();
    int mileage = 0;
    FIELD();
    bool running = false;
};

CLASS();
class BaseCar : public Vehicle
{
    R_FRIEND_DECL(BaseCar, Vehicle);
    ECS_COMPONENT_DECL(BaseCar, Vehicle);

    FIELD();
    int horsepower = 150;
    FIELD();
    float fuelLevel = 0.0f;
};

CLASS();
class Sedan : public BaseCar
{
    R_FRIEND_DECL(Sedan, BaseCar);
    ECS_COMPONENT_DECL(Sedan, BaseCar);

    FIELD();
    int doors = 4;
    FIELD();
    bool hasSunroof = false;
};

CLASS();
class BasePart : public Core::BaseComponent
{
    R_FRIEND_DECL(BasePart, Core::BaseComponent);
    ECS_COMPONENT_DECL(BasePart, Core::BaseComponent);

    FIELD();
    int id = 0;
    FIELD();
    std::string manufacturer = "Generic";
};

CLASS();
class Engine : public BasePart
{
    R_FRIEND_DECL(Engine, BasePart);
    ECS_COMPONENT_DECL(Engine, BasePart);

    FIELD();
    int cylinders = 4;
    FIELD();
    float volume = 2.0f;
    FIELD();
    bool started = false;
};

CLASS();
class TurboEngine : public Engine
{
    R_FRIEND_DECL(TurboEngine, Engine);
    ECS_COMPONENT_DECL(TurboEngine, Engine);

    FIELD();
    float boostPressure = 1.0f;
    FIELD();
    bool turboEnabled = true;
};

CLASS();
class Wheel : public BasePart
{
    R_FRIEND_DECL(Wheel, BasePart);
    ECS_COMPONENT_DECL(Wheel, BasePart);

    FIELD();
    float radius = 17.0f;
    FIELD();
    float pressure = 2.2f;
};

CLASS();
class Interior : public BasePart
{
    R_FRIEND_DECL(Interior, BasePart);
    ECS_COMPONENT_DECL(Interior, BasePart);

    FIELD();
    std::string color = "black";
};

CLASS();
class Seat : public Interior
{
    R_FRIEND_DECL(Seat, Interior);
    ECS_COMPONENT_DECL(Seat, Interior);

    FIELD();
    bool heated = false;
    FIELD();
    int position = 0;
};

CLASS();
class Dashboard : public Interior
{
    R_FRIEND_DECL(Dashboard, Interior);
    ECS_COMPONENT_DECL(Dashboard, Interior);

    FIELD();
    bool hasDisplay = true;
    FIELD();
    int brightness = 50;
};

CLASS();
class Electronics : public BasePart
{
    R_FRIEND_DECL(Electronics, BasePart);
    ECS_COMPONENT_DECL(Electronics, BasePart);

    FIELD();
    int firmwareVersion = 1;
};

CLASS();
class Sensor : public Electronics
{
    R_FRIEND_DECL(Sensor, Electronics);
    ECS_COMPONENT_DECL(Sensor, Electronics);

    FIELD();
    std::string type = "generic";
    FIELD();
    float value = 0.0f;
};

CLASS();
class Camera : public Sensor
{
    R_FRIEND_DECL(Camera, Sensor);
    ECS_COMPONENT_DECL(Camera, Sensor);

    FIELD();
    int resolution = 1080;
    FIELD();
    bool hdr = false;
};

#include "DummyComponent.generated.h" // added by the code generator. Better don't move it.
