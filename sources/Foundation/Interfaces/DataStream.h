// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/String.h"

#include <filesystem>
#include <type_traits>

namespace Foundation
{
    struct IDataIO
    {
        IDataIO() = default;
        IDataIO(const IDataIO&) = default;
        IDataIO(IDataIO&&) = default;
        IDataIO& operator=(const IDataIO&) = default;
        IDataIO& operator=(IDataIO&&) = default;
        virtual ~IDataIO() = default;

        [[nodiscard]] virtual std::filesystem::path getCacheDir() const;
        [[nodiscard]] virtual Core::StringAtom getCacheHash() const = 0;
    };

    template<class T>
    concept IsDataIO = std::derived_from<std::remove_reference_t<T>, IDataIO>;

} // namespace Foundation
