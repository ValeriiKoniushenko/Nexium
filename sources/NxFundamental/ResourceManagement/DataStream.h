// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/IntrusivePtr.h"
#include "Core/Singleton.h"
#include "Foundation/BaseLog.h"
#include "Foundation/Interfaces/DataStream.h"
#include "JustReflectMe/Adapter.h"
#include "nlohmann/json.hpp"

#include <fstream>
#include <functional>

namespace NX
{
    class DataStream;

    class CacheSystem : public Foundation::BaseLog, public Core::Singleton<CacheSystem>
    {
        SINGLETONS_FRIEND(CacheSystem);

    public:
        ~CacheSystem() override = default;

        template<Foundation::IsDataIO T>
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

        void write(const Foundation::IDataIO& data, const nlohmann::json& json);

        template<Foundation::IsDataIO T>
        void read(T& data)
        {
            try
            {
                RResourceStream<RJsonResourceStream> s;
                s.getData() = nlohmann::json::parse(
                    [this, &data]() -> std::string
                    {
                        auto path = getPath(data);
                        std::ifstream ifs(path);
                        if (!ifs)
                        {
                            warnLog("Can't read cache for this object {}. Path: {}. Details: {}"_f
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
                warnLog("Can't read the object from cache: {} {}. Details: {}"_f
                        << data.getCacheDir().generic_string() << data.getCacheHash() << ex.what());
            }
            catch (...)
            {
                warnLog("Can't read the object from cache: {} {}. Due to unknown reasons."_f
                        << data.getCacheDir().generic_string() << data.getCacheHash());
            }
        }

        template<Foundation::IsDataIO T>
        bool tryRead(T& data)
        {
            if (hasCache(data))
            {
                read<T>(data);
                return true;
            }
            return false;
        }

        [[nodiscard]] bool hasCache(const Foundation::IDataIO& data) const;

        void clearCache(const Foundation::IDataIO& data);

        [[nodiscard]] spdlog::logger* getLogger() const override;

    private:
        [[nodiscard]] std::filesystem::path getPath(const Foundation::IDataIO& data) const;
        [[nodiscard]] std::filesystem::path getCachePath(const Foundation::IDataIO& data) const;
        [[nodiscard]] bool createCacheDirIfNotExist(const Foundation::IDataIO& data) const;
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
            std::unordered_map<Core::StringAtom, uint32_t> field;

            [[nodiscard]] bool checkField(const Core::StringAtom& fieldName,
                                          uint32_t flag) const noexcept;
        };

        IDataUpdateBridge() = default;
        IDataUpdateBridge(const IDataUpdateBridge&) = default;
        IDataUpdateBridge(IDataUpdateBridge&&) noexcept = default;
        IDataUpdateBridge& operator=(const IDataUpdateBridge&) = default;
        IDataUpdateBridge& operator=(IDataUpdateBridge&&) noexcept = default;
        virtual ~IDataUpdateBridge() = default;

        virtual void ioFieldsUpdate(DataStream& out) = 0;
        [[nodiscard]] virtual Core::StringAtom getCacheHash() const = 0;

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
        DataStream(const DataStream&) = default;
        DataStream(DataStream&&) noexcept = default;
        DataStream& operator=(const DataStream&) = default;
        DataStream& operator=(DataStream&&) noexcept = default;

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

        [[nodiscard]] Json& getRaw() { return finalJson(); }
        [[nodiscard]] const Json& getRaw() const { return finalJson(); }

        [[nodiscard]] DataStream dedicatedNesting(const char* key);

        void tryPushBackEmptyArrayElement();

    protected:
        struct DataProvider : public Core::IntrusiveRefCounter<DataProvider>
        {
            std::vector<std::pair<Result, std::string>> errors;
            Json json;
            Mode mode = Mode::Input;
        };

        DataStream(const Core::IntrusivePtr<DataProvider>& viewing,
                   const Core::StringAtom& nesting);

        Json& finalJson();
        [[nodiscard]] const Json& finalJson() const
        {
            return const_cast<DataStream*>(this)->finalJson();
        }

    protected:
        Core::IntrusivePtr<DataProvider> _data;

        std::string _extraNestingKey;
        bool _isViewer = false;
    };

} // namespace NX

#include "DataStream.generated.h"
