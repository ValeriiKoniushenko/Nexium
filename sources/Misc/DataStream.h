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

#include "Core/String.h"
#include "nlohmann/json.hpp"

#include <functional>

namespace Core
{

    class DataStream
    {
    public:
        enum class Mode
        {
            Input,
            Output
        };

        enum class Result
        {
            Success,
            ReadFailed,
            WriteFailed,
            InvalidPassedData,
            CustomProcessingError
        };

        using Json = nlohmann::json;

    public:
        DataStream() = default;
        virtual ~DataStream() = default;

        void setMode(Mode mode) noexcept { _mode = mode; }
        [[nodiscard]] Mode getMode() const noexcept { return _mode; }

        template<class T>
        Result updateField(const char* key, T& field)
        {
            if (!key)
            {
                _errors.emplace_back(Result::InvalidPassedData, "nullptr");
                return Result::InvalidPassedData;
            }

            if (_mode == Mode::Input)
            {
                if (!contains(key))
                {
                    _errors.emplace_back(Result::ReadFailed, key);
                    return Result::ReadFailed;
                }

                field = get<T>(key);
            }
            else
            {
                _json[key] = field;
            }

            return Result::Success;
        }

        template<class T, class ReaderFunc, class WriterFunc>
        Result updateField(const char* key, T& field, ReaderFunc&& reader, WriterFunc&& writer)
        {
            if (!key)
            {
                _errors.emplace_back(Result::InvalidPassedData, "nullptr");
                return Result::InvalidPassedData;
            }

            try
            {
                if (_mode == Mode::Input)
                {
                    if (!contains(key))
                    {
                        _errors.emplace_back(Result::ReadFailed, key);
                        return Result::ReadFailed;
                    }

                    reader(field, _json[key]);
                }
                else
                {
                    _json[key] = writer(field);
                }
            }
            catch (const std::exception& er)
            {
                _errors.emplace_back(Result::CustomProcessingError, er.what());
                return Result::CustomProcessingError;
            }
            catch (...)
            {
                _errors.emplace_back(Result::CustomProcessingError, "Undefined internal error");
                return Result::CustomProcessingError;
            }

            return Result::Success;
        }

        template<class T>
        [[nodiscard]] T get(const char* key)
        {
            return _json[key].get<T>();
        }

        [[nodiscard]] bool contains(const char* key) const;

        [[nodiscard]] bool hasErrors() const noexcept { return !_errors.empty(); }
        [[nodiscard]] const std::vector<std::pair<Result, std::string>>& getErrors() const noexcept
        {
            return _errors;
        }

        [[nodiscard]] Json& getRaw() noexcept { return _json; }
        [[nodiscard]] const Json& getRaw() const noexcept { return _json; }

    protected:
        std::vector<std::pair<Result, std::string>> _errors;
        Json _json;
        Mode _mode = Mode::Input;
    };

    struct IDataStreamBridge
    {
        virtual ~IDataStreamBridge() = default;

        virtual void ioFieldsUpdate(DataStream& stream) = 0;

        [[nodiscard]] bool hasCache() const;

        void writeToCache();
        void readFromCache();
        void tryReadFromCache();

        void clearCache();

    protected:
        [[nodiscard]] virtual std::filesystem::path getCacheDir() const { return { "configs" }; }
        [[nodiscard]] virtual StringAtom getCacheHash() const = 0;
        [[nodiscard]] std::filesystem::path getTargetCachePath() const;
    };
} // namespace Core