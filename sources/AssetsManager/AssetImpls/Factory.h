

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

#include "AssetsManager/ECSAsset.h"
#include "Core/Singleton.h"
#include "Core/String.h"

#include <unordered_map>

namespace Core::AssetImpl
{

    class Factory final : public Singleton<Factory>
    {
        SINGLETONS_FRIEND(Factory)
    public:
        [[nodiscard]] std::unique_ptr<ECSAssetImpl> trySpawnImpl(std::type_index i)
        {
            const auto it = _map.find(i);
            if (it != _map.cend())
            {
                return it->second();
            }

            return nullptr;
        }

        template<IsAssetImpl T>
        void registerAssetType()
        {
            if (_map.contains(typeid(typename T::implementedAssetType)))
            {
                globalLog.errorLog(
                    "[AssetImpl::Factory] Asset type {} already registered. You are trying re-register it. Your action is canceled."_f
                    << typeid(typename T::implementedAssetType).name());
                return;
            }

            _map[typeid(typename T::implementedAssetType)]
                = []() -> std::unique_ptr<ECSAssetImpl> { return std::make_unique<T>(); };

            globalLog.infoLog("[AssetImpl::Factory] Asset type {} has registered."_f
                              << typeid(typename T::implementedAssetType).name());
        }

    private:
        static bool _BuiltinAssetTypesRegistration;

        std::unordered_map<std::type_index, std::unique_ptr<ECSAssetImpl> (*)()> _map;
    };

    [[nodiscard]] Factory& GetFactory();

} // namespace Core::AssetImpl
