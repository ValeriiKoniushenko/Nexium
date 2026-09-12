// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/String.h"
#include "Foundation/BaseLog.h"
#include "JustReflectMe/Adapter.h"
#include "NxFundamental/ResourceManagement/DataStream.h"
#include "nlohmann/json.hpp"

namespace NX
{

    // TODO: remove this class. It's unsafe
    CLASS();
    class BaseAsset : public Foundation::BaseLog
    {
        R_FRIEND(BaseAsset);

    public:
        explicit BaseAsset(Core::StringAtom logicPath)
            : _logicPath(std::move(logicPath))
        {
            Assert(std::filesystem::path(_logicPath.c_str()).is_relative(),
                   "Logic path must be relative. It's ID, it's not a real path.");
        }

        ~BaseAsset() override = default;
        BaseAsset(const BaseAsset&) = delete;
        BaseAsset(BaseAsset&&) noexcept = delete;
        BaseAsset& operator=(const BaseAsset&) = delete;
        BaseAsset& operator=(BaseAsset&&) noexcept = delete;

        [[nodiscard]] bool isLoaded() const { return _refCount > 1; }

        [[nodiscard]] const Core::StringAtom& getLogicPath() const { return _logicPath; }

        void loadRequest();
        void unloadRequest();
        void makeHotReload();

        void attachAndReadFromFile(const std::filesystem::path& path);
        void attachSourceFile(const std::filesystem::path& path);
        void writeToFile();

        [[nodiscard]] spdlog::logger* getLogger() const override;

        [[nodiscard]] virtual nlohmann::json serialize() const;
        virtual void deserialize(RResourceStream<RJsonResourceStream>& data);

    protected:
        virtual void onLoadRequest() = 0;
        virtual void onUnloadRequest() = 0;

        /// @return true if you support hot reload. By default - false
        virtual bool onHotReload() { return false; }
        virtual void ioFieldsUpdate(DataStream& out) {}

    protected:
        std::filesystem::path _assetPath;
        Core::StringAtom _logicPath;
        uint32_t _refCount = 0;

        template<class T>
        friend class AssetRef;
    };

    template<class T>
    concept IsAsset = requires(T) {
        { T::fileExtension };
        requires std::derived_from<std::remove_reference_t<T>, BaseAsset>;
    };

    template<class T>
    class AssetRef
    {
    public:
        using AssetT = T;

    public:
        AssetRef() = default;

        explicit AssetRef(T& asset)
            : _asset(&asset)
        {
            increaseRef();
        }

        explicit AssetRef(T* ptr)
            : _asset(ptr)
        {
            increaseRef();
        }

        AssetRef(const AssetRef& other)
            : _asset(other._asset)
        {
            increaseRef();
        }

        AssetRef(AssetRef&& other) noexcept
            : _asset(other._asset)
        {
            other._asset = nullptr;
        }

        AssetRef& operator=(const AssetRef& other)
        {
            if (this != &other) [[likely]]
            {
                decreaseRef();
                _asset = other._asset;
                increaseRef();
            }
            return *this;
        }

        AssetRef& operator=(AssetRef&& other) noexcept
        {
            if (this != &other) [[likely]]
            {
                decreaseRef();
                _asset = other._asset;

                other._asset = nullptr;
            }
            return *this;
        }

        [[nodiscard]] T* get() { return _asset; }
        [[nodiscard]] const T* get() const { return _asset; }
        [[nodiscard]] T& operator*() { return *_asset; }
        [[nodiscard]] const T& operator*() const { return *_asset; }
        [[nodiscard]] T* operator->() { return _asset; }
        [[nodiscard]] const T* operator->() const { return _asset; }

        [[nodiscard]] operator bool() const noexcept { return _asset != nullptr; }
        [[nodiscard]] bool isValid() const noexcept { return _asset != nullptr; }

        ~AssetRef() { decreaseRef(); }

    private:
        void increaseRef()
        {
            // why 2? The first ref is placing at AssetManager like some dummy asset
            // we just know that it was indexed by the system.
            // With the second ref - the final code wants to use it. So, we must
            // load it.
            if (_asset && ++_asset->_refCount == 2)
            {
                _asset->loadRequest();
            }
        }

        void decreaseRef()
        {
            if (_asset)
            {
                Assert(_asset->_refCount != 0,
                       "Invalid ref count, it will be less than zero - impossible.");

                --_asset->_refCount;

                // why 1? see the logic of increaseRef above. The same sense
                // but with reverse logic.
                if (_asset->_refCount == 1)
                {
                    _asset->unloadRequest();
                }

                if (_asset->_refCount == 0)
                {
                    delete _asset;
                    _asset = nullptr;
                }
            }
        }

    private:
        T* _asset = nullptr;
    };
} // namespace NX

#include "BaseAsset.generated.h" // added by the code generator. Better don't move it.
