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

#include "Core/IntrusivePtr.h"
#include "GameplaySystem/ECS/BaseComponent.h"

#include <filesystem>

namespace Core
{
    class ECSAsset;

    class ECSAssetImpl
    {
    public:
        virtual ~ECSAssetImpl() = default;

        virtual void load(const ECSAsset& asset, const nlohmann::json& assetData) = 0;
        virtual void unload(const ECSAsset& asset) = 0;

    protected:
        ECSAssetImpl() = default;
    };

    template<typename T>
    concept IsAssetImpl = std::derived_from<T, ECSAssetImpl>;

    class ECSAsset : public BaseLog, public IntrusiveRefCounter<ECSAsset>
    {
    public:
        inline static const char* fileExtension = ".nx";

        // clang-format off
        CreateEnum(Status, int,
            NotLoaded,      // Absolutely not loaded. Asset's type is undefined
            PreLoaded,      // Loaded only the main information: type & ID & name
            Loaded,         // Full asset's data is loaded (corresponding to asset's type)
            PreLoadingError,// Error while preloading data
            LoadingError    // Error while loading of the main data
        );
        // clang-format on

        struct AssetData
        {
            AssetData() = delete;

            static constexpr const char* type = "type";
            static constexpr const char* name = "name";
            static constexpr const char* data = "data";
            static constexpr const char* assetData = "assetData";
        };

    public:
        explicit ECSAsset(const StringAtom& logicPath)
            : _logicPath(logicPath)
        {
            Assert(_logicPath.isStatic());
        }
        ~ECSAsset() override;

        ECSAsset(const ECSAsset&) = delete;
        ECSAsset& operator=(const ECSAsset&) = delete;
        ECSAsset(ECSAsset&&) = delete;
        ECSAsset& operator=(ECSAsset&&) = delete;

        [[nodiscard]] spdlog::logger* getLogger() const override;

        void connectSourceFile(const std::filesystem::path& src);

        [[nodiscard]] Status getLoadingStatus() const noexcept { return _status; }

        [[nodiscard]] const std::filesystem::path& getSourceFile() const noexcept;

        [[nodiscard]] const StringAtom& getName() const noexcept { return _name; }
        [[nodiscard]] const StringAtom& getType() const noexcept { return _type; }
        [[nodiscard]] const StringAtom& getLogicPath() const noexcept { return _logicPath; }

        [[nodiscard]] BaseComponent::Ptr getData() const noexcept { return _data; }

        [[nodiscard]] nlohmann::json getAssetData() const;

        void syncAssetWithMemory(const nlohmann::json& assetData);

        [[nodiscard]] int getAdapterIndex() const noexcept { return _adapterIndex; }

    protected:
        void load();
        void unload();

        void onIncrementRef(uint32_t count) override;
        void onDecrementRef(uint32_t count) override;

    private:
        void extrudeAndValidateMainDataFromFile();
        void localClear();

    protected:
        std::filesystem::path _pathToSource;
        StringAtom _logicPath;

        StringAtom _name;
        StringAtom _type;

        BaseComponent::Ptr _data;
        std::unique_ptr<ECSAssetImpl> _impl;

        int _adapterIndex = -1;

    private:
        Status _status = Status::NotLoaded;
    };

    using NXAsset = IntrusivePtr<ECSAsset>;

} // namespace Core