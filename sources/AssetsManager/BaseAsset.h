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
#include "Misc/BaseLog.h"
#include "Misc/DataStream.h"
#include "ModuleInfo.h"
#include "nlohmann/json.hpp"

namespace Core
{
    class BaseAsset : public BaseLog
    {
    public:
        explicit BaseAsset(const StringAtom& logicPath)
            : _logicPath(logicPath)
        {
            DEBUG_ASSERT(std::filesystem::path(_logicPath.c_str()).is_relative(),
                         "Logic path must be relative. It's ID, it's not a real path.");
        }

        ~BaseAsset() override = default;

        [[nodiscard]] bool isLoaded() const { return _refCount > 1; }

        [[nodiscard]] const StringAtom& getLogicPath() const { return _logicPath; }

        virtual void onLoadRequest() = 0;
        virtual void onUnloadRequest() = 0;

        void attachAndReadFromFile(const std::filesystem::path& path);
        void writeToFile();

        [[nodiscard]] spdlog::logger* getLogger() const override
        {
            return AssetsManager::getLogger();
        }

    protected:
        virtual void ioFieldsUpdate(DataStream& stream) = 0;

    protected:
        std::filesystem::path _assetPath;
        StringAtom _logicPath;
        uint32_t _refCount = 0;

        template<class T>
        friend class AssetRef;
    };

    template<class T>
    concept IsAsset = requires(T) {
        { T::fileExtension };
        requires std::derived_from<T, BaseAsset>;
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
                _asset->onLoadRequest();
            }
        }

        void decreaseRef()
        {
            if (_asset)
            {
                DEBUG_ASSERT(_asset->_refCount != 0,
                             "Invalid ref count, it will be less than zero - impossible.");

                --_asset->_refCount;

                // why 1? see the logic of increaseRef above. The same sense
                // but with reverse logic.
                if (_asset->_refCount == 1)
                {
                    _asset->onUnloadRequest();
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
} // namespace Core