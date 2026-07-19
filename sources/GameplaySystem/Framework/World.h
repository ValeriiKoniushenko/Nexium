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

#include "../../ResourceManagement/DataStream.h"
#include "Core/Color.h"
#include "GameState.h"
#include "LevelData.h"
#include "Misc/BaseLog.h"
#include "PlayerState.h"

namespace Core
{

    CLASS();
    struct LightningProps : public IDataIO
    {
        R_FRIEND(LightningProps);

        FIELD();
        float ambientStrength = 1.f;

        FIELD();
        float minLightStrength = 0.2f;

        FIELD();
        float specularStrength = 0.5f;

        FIELD();
        float specularPow = 32.f;

        FIELD();
        glm::vec3 sunDirection = glm::vec3(0.5f);

        FIELD();
        Core::NormColor3 color = Core::NormColor3(1.f);

        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] StringAtom getCacheHash() const override;
    };

    class World : public BaseLog, public IDataIO
    {
    public:
        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "world"; }

    public:
        float timeDelta = 0.f;

        LightningProps lightning;
        PlayerState playerState;
        GameState gameState;
        LevelData levelData;
        StringAtom worldName = "Default";

        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] StringAtom getCacheHash() const override;
    };

} // namespace Core

#include "World.generated.h" // added by the code generator. Better don't move it.
