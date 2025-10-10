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

    std::filesystem::path LightningProps::getCacheDir() const
    {
        return "LightningProps";
    }

    StringAtom LightningProps::getCacheHash() const
    {
        return "LightningProps";
    }

    void LightningProps::ioFieldsUpdate(DataStream& stream)
    {
        stream.updateField("color", color);
        stream.updateField("minLightStrength", minLightStrength);
        stream.updateField("ambientStrength", ambientStrength);
        stream.updateField("specularStrength", specularStrength);
        stream.updateField("specularPow", specularPow);
        stream.updateField("sunDirection", sunDirection);
    }

    std::filesystem::path World::getCacheDir() const
    {
        return "world";
    }

    StringAtom World::getCacheHash() const
    {
        return worldName;
    }

    void World::ioFieldsUpdate(DataStream& stream)
    {
        stream.updateField("worldName", worldName, "Default"_dyn);
    }

} // namespace Core