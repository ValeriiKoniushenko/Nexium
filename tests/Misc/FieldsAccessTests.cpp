// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "gtest/gtest.h"

class IEditorFieldsFetcher
{
public:
    struct FieldUnit
    {
        std::string name;
        std::string type;
        void* getter = nullptr;
        void* setter = nullptr;
    };

public:
    virtual ~IEditorFieldsFetcher() = default;

protected:
    IEditorFieldsFetcher() = default;
};

class Human : public IEditorFieldsFetcher
{
public:
    Human() = default;

    [[nodiscard]] const std::string& getName() const noexcept { return _name; }
    [[nodiscard]] int getAge() const noexcept { return _age; }

    void setName(const std::string& value) noexcept { _name = value; }
    void setAge(int value) noexcept { _age = value; }

private:
    std::string _name = "Anton";
    int _age = 21;
};

TEST(FieldsAccessTests, Interface)
{
    Human h;
}