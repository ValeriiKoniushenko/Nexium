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

#pragma once

#include "Core/Color.h"
#include "GameState.h"
#include "LevelData.h"
#include "Misc/BaseLog.h"
#include "Misc/JsonCacheable.h"
#include "ModuleInfo.h"
#include "PlayerState.h"

namespace Core
{
    struct LightningProps : public JsonAdapter
    {
        glm::vec3 position = glm::vec3(1'000'000.f, 1'000'000.f, 1'000'000.f);
        NormColor3 color = NormColor3(1.f);
        float ambientStrength = 1.f;
        float minLightStrength = 0.2f;
        float specularStrength = 0.5f;
        int specularPow = 32;

        [[nodiscard]] nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren) override;
    };

    class World : public BaseLog, public JsonCacheable, public Utils::NotCopyableAndNotMoveable
    {
    public:
        [[nodiscard]] spdlog::logger* getLogger() const override { return Framework::getLogger(); }
        [[nodiscard]] const char* getPrefix() const override { return "World"; }

    protected:
        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] StringAtom getCacheHash() const override;
        [[nodiscard]] nlohmann::json toCacheData() const override;
        void fromCacheData(const nlohmann::json& json) override;

    public:
        float timeDelta = 0.f;

        LightningProps lightning;
        PlayerState playerState;
        GameState gameState;
        LevelData levelData;
        StringAtom worldName = "Default";
    };
} // namespace Core