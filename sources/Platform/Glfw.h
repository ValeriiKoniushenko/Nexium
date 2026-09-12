// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#define GLFW_INCLUDE_NONE
#ifdef _WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include "GLFW/glfw3.h"

#ifdef _WIN32
    #include "GLFW/glfw3native.h"
#endif
