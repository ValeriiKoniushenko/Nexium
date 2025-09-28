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
#include <Core/String.h>
#include <nlohmann/json_fwd.hpp>

namespace Core
{
    class JsonCacheable
    {
    public:
        virtual ~JsonCacheable() = default;

        /**
         * @brief Generate in your own way a possible unique string.
         * After that, you can find your logs with such stem(name).
         */
        void writeToCache() const;

        [[nodiscard]] bool hasCache() const;

        void readFromCache();

        void tryReadFromCache();

        void clearCache();

    protected:
        JsonCacheable() = default;

        [[nodiscard]] virtual std::filesystem::path getCacheDir() const { return { "configs" }; }

        [[nodiscard]] virtual StringAtom getCacheHash() const = 0;

        [[nodiscard]] virtual nlohmann::json toCacheData() const = 0;

        virtual void fromCacheData(const nlohmann::json& json) = 0;

        [[nodiscard]] std::filesystem::path getTargetPath() const;
    };
} // namespace Core