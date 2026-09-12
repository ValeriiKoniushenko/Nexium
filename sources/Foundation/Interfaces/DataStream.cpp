// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "DataStream.h"

#include "Foundation/Configs.h"

namespace Foundation
{

    std::filesystem::path IDataIO::getCacheDir() const
    {
        return Foundation::Config::Path::cacheDir;
    }

} // namespace Foundation
