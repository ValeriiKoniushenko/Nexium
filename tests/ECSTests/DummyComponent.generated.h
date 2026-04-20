/*
 * This code was generated automatically with
 * https://github.com/ValeriiKoniushenko/JustReflectMe
 *
 * DO NOT EDIT MANUALLY!
 * Your changes will be replaced next time
 */

// clang-format off
#include <string>
#include <string_view>
#include <vector>

template<>
struct R<DummyComponent>
{
    static constexpr std::string_view Name() { return "DummyComponent"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "int", "a" },
			{ "std::string", "name" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const DummyComponent& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("a", obj.a);
		s.write("name", obj.name);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const DummyComponent& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("a", obj.a);
		s.write("name", obj.name);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, DummyComponent& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
		s.read("a", obj.a, 123);
		s.read("name", obj.name, "Lola");
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<DummyComponent>

template<>
struct R<HardConstructorComponent>
{
    static constexpr std::string_view Name() { return "HardConstructorComponent"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 0; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const HardConstructorComponent& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const HardConstructorComponent& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, HardConstructorComponent& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<HardConstructorComponent>

template<>
struct R<InitSpyComponent>
{
    static constexpr std::string_view Name() { return "InitSpyComponent"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 0; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const InitSpyComponent& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const InitSpyComponent& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, InitSpyComponent& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<InitSpyComponent>

template<>
struct R<Vehicle>
{
    static constexpr std::string_view Name() { return "Vehicle"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 3; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::string", "vin" },
			{ "int", "mileage" },
			{ "bool", "running" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Vehicle& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("vin", obj.vin);
		s.write("mileage", obj.mileage);
		s.write("running", obj.running);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Vehicle& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("vin", obj.vin);
		s.write("mileage", obj.mileage);
		s.write("running", obj.running);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Vehicle& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
		s.read("vin", obj.vin, "UNKNOWN");
		s.read("mileage", obj.mileage, 0);
		s.read("running", obj.running, false);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Vehicle>

template<>
struct R<BaseCar>
{
    static constexpr std::string_view Name() { return "BaseCar"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "int", "horsepower" },
			{ "float", "fuelLevel" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const BaseCar& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Vehicle>::Serialize<RImpl>(obj).getData());
		s.write("horsepower", obj.horsepower);
		s.write("fuelLevel", obj.fuelLevel);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const BaseCar& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Vehicle>::Serialize<RImpl>(obj).getData());
		s.write("horsepower", obj.horsepower);
		s.write("fuelLevel", obj.fuelLevel);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, BaseCar& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Vehicle>::Deserialize<RImpl>(s, obj, true);
		s.read("horsepower", obj.horsepower, 150);
		s.read("fuelLevel", obj.fuelLevel, 0.0f);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<BaseCar>

template<>
struct R<Sedan>
{
    static constexpr std::string_view Name() { return "Sedan"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "int", "doors" },
			{ "bool", "hasSunroof" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Sedan& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<BaseCar>::Serialize<RImpl>(obj).getData());
		s.write("doors", obj.doors);
		s.write("hasSunroof", obj.hasSunroof);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Sedan& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<BaseCar>::Serialize<RImpl>(obj).getData());
		s.write("doors", obj.doors);
		s.write("hasSunroof", obj.hasSunroof);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Sedan& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<BaseCar>::Deserialize<RImpl>(s, obj, true);
		s.read("doors", obj.doors, 4);
		s.read("hasSunroof", obj.hasSunroof, false);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Sedan>

template<>
struct R<BasePart>
{
    static constexpr std::string_view Name() { return "BasePart"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "int", "id" },
			{ "std::string", "manufacturer" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const BasePart& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("id", obj.id);
		s.write("manufacturer", obj.manufacturer);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const BasePart& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Core::BaseComponent>::Serialize<RImpl>(obj).getData());
		s.write("id", obj.id);
		s.write("manufacturer", obj.manufacturer);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, BasePart& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Core::BaseComponent>::Deserialize<RImpl>(s, obj, true);
		s.read("id", obj.id, 0);
		s.read("manufacturer", obj.manufacturer, "Generic");
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<BasePart>

template<>
struct R<Engine>
{
    static constexpr std::string_view Name() { return "Engine"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 3; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "int", "cylinders" },
			{ "float", "volume" },
			{ "bool", "started" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Engine& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<BasePart>::Serialize<RImpl>(obj).getData());
		s.write("cylinders", obj.cylinders);
		s.write("volume", obj.volume);
		s.write("started", obj.started);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Engine& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<BasePart>::Serialize<RImpl>(obj).getData());
		s.write("cylinders", obj.cylinders);
		s.write("volume", obj.volume);
		s.write("started", obj.started);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Engine& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<BasePart>::Deserialize<RImpl>(s, obj, true);
		s.read("cylinders", obj.cylinders, 4);
		s.read("volume", obj.volume, 2.0f);
		s.read("started", obj.started, false);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Engine>

template<>
struct R<TurboEngine>
{
    static constexpr std::string_view Name() { return "TurboEngine"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "float", "boostPressure" },
			{ "bool", "turboEnabled" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const TurboEngine& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Engine>::Serialize<RImpl>(obj).getData());
		s.write("boostPressure", obj.boostPressure);
		s.write("turboEnabled", obj.turboEnabled);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const TurboEngine& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Engine>::Serialize<RImpl>(obj).getData());
		s.write("boostPressure", obj.boostPressure);
		s.write("turboEnabled", obj.turboEnabled);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, TurboEngine& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Engine>::Deserialize<RImpl>(s, obj, true);
		s.read("boostPressure", obj.boostPressure, 1.0f);
		s.read("turboEnabled", obj.turboEnabled, true);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<TurboEngine>

template<>
struct R<Wheel>
{
    static constexpr std::string_view Name() { return "Wheel"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "float", "radius" },
			{ "float", "pressure" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Wheel& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<BasePart>::Serialize<RImpl>(obj).getData());
		s.write("radius", obj.radius);
		s.write("pressure", obj.pressure);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Wheel& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<BasePart>::Serialize<RImpl>(obj).getData());
		s.write("radius", obj.radius);
		s.write("pressure", obj.pressure);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Wheel& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<BasePart>::Deserialize<RImpl>(s, obj, true);
		s.read("radius", obj.radius, 17.0f);
		s.read("pressure", obj.pressure, 2.2f);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Wheel>

template<>
struct R<Interior>
{
    static constexpr std::string_view Name() { return "Interior"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::string", "color" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Interior& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<BasePart>::Serialize<RImpl>(obj).getData());
		s.write("color", obj.color);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Interior& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<BasePart>::Serialize<RImpl>(obj).getData());
		s.write("color", obj.color);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Interior& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<BasePart>::Deserialize<RImpl>(s, obj, true);
		s.read("color", obj.color, "black");
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Interior>

template<>
struct R<Seat>
{
    static constexpr std::string_view Name() { return "Seat"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "bool", "heated" },
			{ "int", "position" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Seat& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Interior>::Serialize<RImpl>(obj).getData());
		s.write("heated", obj.heated);
		s.write("position", obj.position);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Seat& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Interior>::Serialize<RImpl>(obj).getData());
		s.write("heated", obj.heated);
		s.write("position", obj.position);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Seat& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Interior>::Deserialize<RImpl>(s, obj, true);
		s.read("heated", obj.heated, false);
		s.read("position", obj.position, 0);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Seat>

template<>
struct R<Dashboard>
{
    static constexpr std::string_view Name() { return "Dashboard"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "bool", "hasDisplay" },
			{ "int", "brightness" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Dashboard& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Interior>::Serialize<RImpl>(obj).getData());
		s.write("hasDisplay", obj.hasDisplay);
		s.write("brightness", obj.brightness);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Dashboard& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Interior>::Serialize<RImpl>(obj).getData());
		s.write("hasDisplay", obj.hasDisplay);
		s.write("brightness", obj.brightness);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Dashboard& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Interior>::Deserialize<RImpl>(s, obj, true);
		s.read("hasDisplay", obj.hasDisplay, true);
		s.read("brightness", obj.brightness, 50);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Dashboard>

template<>
struct R<Electronics>
{
    static constexpr std::string_view Name() { return "Electronics"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 1; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "int", "firmwareVersion" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Electronics& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<BasePart>::Serialize<RImpl>(obj).getData());
		s.write("firmwareVersion", obj.firmwareVersion);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Electronics& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<BasePart>::Serialize<RImpl>(obj).getData());
		s.write("firmwareVersion", obj.firmwareVersion);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Electronics& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<BasePart>::Deserialize<RImpl>(s, obj, true);
		s.read("firmwareVersion", obj.firmwareVersion, 1);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Electronics>

template<>
struct R<Sensor>
{
    static constexpr std::string_view Name() { return "Sensor"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "std::string", "type" },
			{ "float", "value" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Sensor& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Electronics>::Serialize<RImpl>(obj).getData());
		s.write("type", obj.type);
		s.write("value", obj.value);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Sensor& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Electronics>::Serialize<RImpl>(obj).getData());
		s.write("type", obj.type);
		s.write("value", obj.value);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Sensor& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Electronics>::Deserialize<RImpl>(s, obj, true);
		s.read("type", obj.type, "generic");
		s.read("value", obj.value, 0.0f);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Sensor>

template<>
struct R<Camera>
{
    static constexpr std::string_view Name() { return "Camera"; }
    static constexpr std::string_view ParentScope() { return ""; }
    static constexpr std::size_t GetFieldNumbers() { return 2; }
    static constexpr std::vector<RClassField> GetFields() {
        return {
			{ "int", "resolution" },
			{ "bool", "hdr" },
		};
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    [[nodiscard]] static RResourceStream<RImpl> Serialize(const Camera& obj, bool noSignals = false)
    {
        RResourceStream<RImpl> s;
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Sensor>::Serialize<RImpl>(obj).getData());
		s.write("resolution", obj.resolution);
		s.write("hdr", obj.hdr);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
        return s;
    }

    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Serialize(const Camera& obj, RResourceStream<RImpl>& s, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreSerialize(obj);
        }

		s.write(R<Sensor>::Serialize<RImpl>(obj).getData());
		s.write("resolution", obj.resolution);
		s.write("hdr", obj.hdr);
        if (!noSignals)
        {
            _RTryCallPostSerialize(obj, s.logs());
        }
    }


    template<IsResourceStreamImpl RImpl = RJsonResourceStream>
    static void Deserialize(const RResourceStream<RImpl>& s, Camera& obj, bool noSignals = false)
    {
        if (!noSignals)
        {
            _RTryCallPreDeserialize(obj);
        }

		R<Sensor>::Deserialize<RImpl>(s, obj, true);
		s.read("resolution", obj.resolution, 1080);
		s.read("hdr", obj.hdr, false);
        if (!noSignals)
        {
            _RTryCallPostDeserialize(obj, s.logs());
        }
    }
}; // struct R<Camera>
// clang-format on
