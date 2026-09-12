// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Singleton.h"
#include "Core/String.h"
#include "ECSAsset.h"

#include <unordered_map>

namespace NX
{

    class Factory final : public Core::Singleton<Factory>
    {
        SINGLETONS_FRIEND(Factory)
    public:
        ~Factory() override = default;

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
                gGlobalLog.errorLog(
                    "[AssetImpl::Factory] Asset type {} already registered. You are trying re-register it. Your action is canceled."_f
                    << R<typename T::implementedAssetType>::FullName());
                return;
            }

            _map[typeid(typename T::implementedAssetType)]
                = []() -> std::unique_ptr<ECSAssetImpl> { return std::make_unique<T>(); };

            gGlobalLog.infoLog("[AssetImpl::Factory] Asset type {} has registered."_f
                               << R<typename T::implementedAssetType>::FullName());
        }

    private:
        static bool _BuiltinAssetTypesRegistration;

        std::unordered_map<std::type_index, std::unique_ptr<ECSAssetImpl> (*)()> _map;
    };

    [[nodiscard]] Factory& GetFactory();

} // namespace NX
