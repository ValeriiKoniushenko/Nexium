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

#include "World.h"

namespace Core
{
    nlohmann::json LightningProps::toJson() const
    {
        nlohmann::json json;

        json["position"] = position;
        json["color"] = color;
        json["ambientStrength"] = ambientStrength;
        json["specularStrength"] = specularStrength;
        json["specularPow"] = specularPow;

        return json;
    }

    void LightningProps::fromJson(const nlohmann::json& json, bool isIgnoreChildren)
    {
        tryReadJsonTo(position, "position", json);
        tryReadJsonTo(color, "color", json);
        tryReadJsonTo(ambientStrength, "ambientStrength", json);
        tryReadJsonTo(specularStrength, "specularStrength", json);
        tryReadJsonTo(specularPow, "specularPow", json);
    }

    std::filesystem::path World::getCacheDir() const
    {
        return JsonCacheable::getCacheDir() / "World";
    }

    StringAtom World::getCacheHash() const
    {
        return worldName;
    }

    nlohmann::json World::toCacheData() const
    {
        nlohmann::json json;

        json["worldName"] = worldName;
        json["lightning"] = lightning.toJson();

        return json;
    }

    void World::fromCacheData(const nlohmann::json& json)
    {
        if (json.contains("worldName"))
        {
            worldName = json["worldName"].get<StringAtom>();
        }
        if (json.contains("lightning"))
        {
            lightning.fromJson(json["lightning"], false);
        }
    }
} // namespace Core