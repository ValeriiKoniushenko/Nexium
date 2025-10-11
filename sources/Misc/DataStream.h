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

#include "Configs.h"
#include "Core/String.h"
#include "Misc/JsonAdapter.h"

#include <functional>

namespace Core
{
    class DataStream;

    struct IDataUpdateBridge
    {
        virtual ~IDataUpdateBridge() = default;

        virtual void ioFieldsUpdate(DataStream& stream) = 0;
        [[nodiscard]] virtual StringAtom getCacheHash() const = 0;
    };

    struct IDataStreamBridge : public IDataUpdateBridge
    {
        [[nodiscard]] bool hasCache() const;

        void writeToCache();
        void readFromCache();
        void tryReadFromCache();

        void clearCache();

        [[nodiscard]] std::filesystem::path getTargetCachePath() const;
        [[nodiscard]] std::filesystem::path getTargetCacheDirPath() const;

    protected:
        [[nodiscard]] virtual std::filesystem::path getCacheDir() const { return {}; }
    };

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

        /**
         * @tparam T data type. Can be any data that is convenient for you.
         * @param key name inside a cache system
         * @param field data type that will be read/write
         * @param defaultValue default value
         * @return the result of an operation
         */
        template<class T>
        Result field(const char* key, T& field, const T& defaultValue = {})
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
                    field = defaultValue;
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

        Result field(IDataUpdateBridge& bridge);

        /**
         * @tparam T data type. Can be any data that is convenient for you.
         * @tparam ReaderFunc should take T& and const nlohmann::json& and manually fetch data to T.
         * No return.
         * @tparam WriterFunc should take const nlohmann::json& process it and return T
         * @param key name inside a cache system
         * @param field data type that will be read/write
         * @param reader callback for reading
         * @param writer callback for writing
         * @return
         */
        template<class T, class ReaderFunc, class WriterFunc>
        Result field(const char* key, T& field, const ReaderFunc& reader, const WriterFunc& writer)
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

        Result nesting(const char* key, const std::function<void(DataStream&)>& callback);
        Result array(const char* key, const std::function<void(DataStream&)>& callback);

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

} // namespace Core