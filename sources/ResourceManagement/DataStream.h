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

#include "Core/IntrusivePtr.h"
#include "Core/Singleton.h"
#include "Core/String.h"
#include "JustReflectMe/Adapter.h"
#include "Misc/BaseLog.h"
#include "nlohmann/json.hpp"

#include <fstream>
#include <functional>
#include <type_traits>

namespace Core
{
    class DataStream;

    struct IDataIO
    {
        IDataIO() = default;
        IDataIO(const IDataIO&) = default;
        IDataIO(IDataIO&&) = default;
        IDataIO& operator=(const IDataIO&) = default;
        IDataIO& operator=(IDataIO&&) = default;
        virtual ~IDataIO() = default;

        [[nodiscard]] virtual std::filesystem::path getCacheDir() const { return "cache"; };
        [[nodiscard]] virtual StringAtom getCacheHash() const = 0;
    };

    template<class T>
    concept IsDataIO = std::derived_from<std::remove_reference_t<T>, IDataIO>;

    class CacheSystem : public BaseLog, public Singleton<CacheSystem>
    {
        SINGLETONS_FRIEND(CacheSystem);

    public:
        template<IsDataIO T>
        void write(const T& data)
        {
            if constexpr (requires { data.serialize(); })
            {
                write(data, data.serialize());
            }
            else
            {
                write(data, R<T>::template Serialize<RJsonResourceStream>(data).getData());
            }
        }

        void write(const IDataIO& data, const nlohmann::json& json);

        template<IsDataIO T>
        void read(T& data)
        {
            try
            {
                RResourceStream<RJsonResourceStream> s;
                s.getData() = nlohmann::json::parse(
                    [this, &data]() -> std::string
                    {
                        std::ifstream ifs(getPath(data));
                        if (!ifs)
                        {
                            errorLog("Can't read cache for this object {}. Path: {}. Details: {}"_f
                                     << data.getCacheHash() << getPath(data)
                                     << std::strerror(errno));
                            return {};
                        }

                        return { (std::istreambuf_iterator<char>(ifs)),
                                 std::istreambuf_iterator<char>() };
                    }());

                if constexpr (requires { data.deserialize(s); })
                {
                    data.deserialize(s);
                }
                else
                {
                    R<T>::template Deserialize<RJsonResourceStream>(s, data);
                }
            }
            catch (std::exception& ex)
            {
                errorLog("Can't read the object from cache: {} {}. Details: {}"_f
                         << data.getCacheDir().generic_string() << data.getCacheHash()
                         << ex.what());
            }
            catch (...)
            {
                errorLog("Can't read the object from cache: {} {}. Due to unknown reasons."_f
                         << data.getCacheDir().generic_string() << data.getCacheHash());
            }
        }

        template<IsDataIO T>
        bool tryRead(T& data)
        {
            if (hasCache(data))
            {
                read<T>(data);
                return true;
            }
            return false;
        }

        [[nodiscard]] bool hasCache(const IDataIO& data) const;

        void clearCache(const IDataIO& data);

        [[nodiscard]] spdlog::logger* getLogger() const override;

    private:
        [[nodiscard]] std::filesystem::path getPath(const IDataIO& data) const;
        [[nodiscard]] std::filesystem::path getCachePath(const IDataIO& data) const;
        [[nodiscard]] bool createCacheDirIfNotExist(const IDataIO& data) const;
    };

    [[nodiscard]] inline CacheSystem& GetCacheSystem()
    {
        return CacheSystem::Instance();
    }

    struct IDataUpdateBridge
    {
        enum Rule : uint8_t
        {
            None = 0,
            NoRead = 1 << 1,
            NoWrite = 1 << 2,
            NoReadAndWrite = NoRead | NoWrite,
            // WarningIfCantRead = 1 << 3,
            // ErrorIfCantRead = 1 << 4,
            // CriticalErrorIfCantRead = 1 << 5
        };

        /// Rules for streaming of an object. For the whole object and for
        /// every field (if it's necessary).
        struct Rules final
        {
            uint32_t main = Rule::None;
            std::unordered_map<StringAtom, uint32_t> field;

            [[nodiscard]] bool checkField(const StringAtom& fieldName,
                                          uint32_t flag) const noexcept;
        };

        virtual ~IDataUpdateBridge() = default;

        virtual void ioFieldsUpdate(DataStream& out) = 0;
        [[nodiscard]] virtual StringAtom getCacheHash() const = 0;

        [[nodiscard]] virtual Rules* getRules() const noexcept { return nullptr; }
    };

    class DataStream
    {
    public:
        ENUM_CLASS();
        enum class Mode
        {
            Input,
            Output
        };

        ENUM_CLASS();
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
        DataStream();
        virtual ~DataStream() = default;

        void setMode(Mode mode) noexcept { _data->mode = mode; }
        [[nodiscard]] Mode getMode() const noexcept { return _data->mode; }

        /// @tparam T data type. Can be any data that is convenient for you.
        /// @param key name inside a cache system
        /// @param field data type that will be read/write
        /// @param defaultValue default value
        /// @return the result of an operation
        template<class T>
        Result field(const char* key, T& field, const T& defaultValue = {})
        {
            if (!key)
            {
                _data->errors.emplace_back(Result::InvalidPassedData, "nullptr");
                return Result::InvalidPassedData;
            }

            if (_data->mode == Mode::Input)
            {
                if (!contains(key))
                {
                    field = defaultValue;
                    _data->errors.emplace_back(Result::ReadFailed, key);
                    return Result::ReadFailed;
                }

                field = get<T>(key);
            }
            else
            {
                // finalJson()[key] = field;
            }

            return Result::Success;
        }

        Result field(IDataUpdateBridge& bridge);

        /// @tparam T data type. Can be any data that is convenient for you.
        /// @tparam ReaderFunc should take T& and const nlohmann::json& and manually fetch data to
        /// T. No return.
        /// @tparam WriterFunc should take const nlohmann::json& process it and return T
        /// @param key name inside a cache system
        /// @param field data type that will be read/write
        /// @param reader callback for reading
        /// @param writer callback for writing
        /// @return
        template<class T, class ReaderFunc, class WriterFunc>
        Result field(const char* key, T& field, const ReaderFunc& reader, const WriterFunc& writer)
        {
            if (!key)
            {
                _data->errors.emplace_back(Result::InvalidPassedData, "nullptr");
                return Result::InvalidPassedData;
            }

            try
            {
                if (_data->mode == Mode::Input)
                {
                    if (!contains(key))
                    {
                        _data->errors.emplace_back(Result::ReadFailed, key);
                        return Result::ReadFailed;
                    }

                    reader(field, finalJson()[key]);
                }
                else
                {
                    finalJson()[key] = writer(field);
                }
            }
            catch (const std::exception& er)
            {
                _data->errors.emplace_back(Result::CustomProcessingError, er.what());
                return Result::CustomProcessingError;
            }
            catch (...)
            {
                _data->errors.emplace_back(Result::CustomProcessingError,
                                           "Undefined internal error");
                return Result::CustomProcessingError;
            }

            return Result::Success;
        }

        Result nesting(const char* key, const std::function<void(DataStream&)>& callback);
        Result array(const char* key,
                     const std::function<void(DataStream&, std::size_t)>& callback);

        template<class T>
        [[nodiscard]] T get(const char* key)
        {
            // return finalJson()[key].get<T>();
            return {};
        }

        [[nodiscard]] bool contains(const char* key) const;

        [[nodiscard]] bool hasErrors() const noexcept { return !_data->errors.empty(); }
        [[nodiscard]] const std::vector<std::pair<Result, std::string>>& getErrors() const noexcept
        {
            return _data->errors;
        }

        [[nodiscard]] Json& getRaw() noexcept { return finalJson(); }
        [[nodiscard]] const Json& getRaw() const noexcept { return finalJson(); }

        [[nodiscard]] DataStream dedicatedNesting(const char* key);

        void tryPushBackEmptyArrayElement();

    protected:
        struct DataProvider : public Core::IntrusiveRefCounter<DataProvider>
        {
            std::vector<std::pair<Result, std::string>> errors;
            Json json;
            Mode mode = Mode::Input;
        };

        DataStream(const IntrusivePtr<DataProvider>& viewing, const StringAtom& nesting);

        Json& finalJson();
        [[nodiscard]] const Json& finalJson() const
        {
            return const_cast<DataStream*>(this)->finalJson();
        }

    protected:
        IntrusivePtr<DataProvider> _data;

        std::string _extraNestingKey;
        bool _isViewer = false;
    };

} // namespace Core

#include "DataStream.generated.h"
