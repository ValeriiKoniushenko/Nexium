/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "Misc/DataStream.h"

#include "gtest/gtest.h"

using namespace Core;

namespace
{

    struct Cell
    {
        int a = 1;
        int b = 2;
    };

    class SomeData : public IDataStreamBridge
    {
    public:
        void ioFieldsUpdate(DataStream& stream) override
        {
            stream.field("age", age);
            stream.field("name", name);
            stream.field("vec", vec);
            stream.field(
                "container", container,
                [](std::unordered_map<std::string, Cell>& out, const DataStream::Json& json) -> void
                {
                    for (auto& [key, el] : json.items())
                    {
                        Cell tmp;
                        tmp.a = el["a"].get<int>();
                        tmp.b = el["b"].get<int>();

                        out[key] = tmp;
                    }
                },
                [](const std::unordered_map<std::string, Cell>& out) -> DataStream::Json
                {
                    DataStream::Json json;
                    for (auto&& [key, value] : out)
                    {
                        DataStream::Json cell;
                        cell["a"] = value.a;
                        cell["b"] = value.b;

                        json[key] = cell;
                    }
                    return json;
                });
        }

    protected:
        StringAtom getCacheHash() const override { return "SomeData"_atom; }

    private:
        int age = 19;
        std::string name = "Andriy";
        std::vector<int> vec = { 1, 2, 3, 4 };
        std::unordered_map<std::string, Cell> container
            = { { "first", { 1, 2 } }, { "second", { 3, 4 } }, { "third", { 5, 6 } } };
    };

} // namespace

TEST(DataStreamTests, Output)
{
    DataStream stream;

    std::string str = "World";
    stream.setMode(DataStream::Mode::Output);
    ASSERT_EQ(DataStream::Result::Success, stream.field("key", str));

    ASSERT_TRUE(stream.contains("key"));
    ASSERT_EQ("World", stream.get<std::string>("key"));
}

TEST(DataStreamTests, Input)
{
    DataStream stream;

    {
        std::string str = "World";
        stream.setMode(DataStream::Mode::Output);
        ASSERT_EQ(DataStream::Result::Success, stream.field("key", str));
    }

    std::string str;
    stream.setMode(DataStream::Mode::Input);

    ASSERT_TRUE(str.empty());
    ASSERT_TRUE(stream.contains("key"));
    ASSERT_EQ(DataStream::Result::Success, stream.field("key", str));
    ASSERT_EQ("World", str);
}

TEST(DataStreamTests, CustomClass)
{
    DataStream stream;
    stream.setMode(DataStream::Mode::Output);

    SomeData data;
    data.ioFieldsUpdate(stream);

    ASSERT_FALSE(stream.hasErrors());
    ASSERT_TRUE(stream.contains("name"));
    ASSERT_EQ("Andriy", stream.get<std::string>("name"));
    ASSERT_TRUE(stream.contains("age"));
    ASSERT_EQ(19, stream.get<int>("age"));
    ASSERT_TRUE(stream.contains("vec"));

    stream.setMode(DataStream::Mode::Input);
    data.ioFieldsUpdate(stream);
}