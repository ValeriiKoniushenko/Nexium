// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "../PrivateModuleInfo.h"
#include "Foundation/BaseLog.h"
#include "Foundation/Interfaces/DataStream.h"
#include "InputAction.h"

#include <unordered_map>

namespace NX
{
    template<IsInputAction InputTParam>
    class InputManger : public Foundation::BaseLog, public Foundation::IDataIO
    {
        INTRUSIVE_PTR_ADAPTERS(InputManger);

    public:
        using InputT = InputTParam;
        using MappingT = std::unordered_map<Core::StringAtom, Core::IntrusivePtr<InputT>>;

        InputManger() = default;

        ~InputManger() override = default;
        InputManger(const InputManger&) = delete;
        InputManger(InputManger&&) noexcept = delete;
        InputManger& operator=(const InputManger&) = delete;
        InputManger& operator=(InputManger&&) noexcept = delete;

        virtual void update()
        {
            for (auto&& [_, ia] : _mapping)
            {
                ia->update();
            }
        }

        [[nodiscard]] const MappingT& getMapping() const noexcept { return _mapping; }

        [[nodiscard]] InputT::Ptr getOrCreate(const Core::StringAtom& name, InputT::KeyT key)
        {
            if (isExist(name))
            {
                return _mapping[name];
            }

            return create(name, key);
        }

        [[nodiscard]] bool isExist(InputT::KeyT key) const { return !!impl_get<true>(this, key); }

        [[nodiscard]] bool isExist(const Core::StringAtom& name) const { return !!get(name); }

        [[nodiscard]] typename InputT::Ptr get(typename InputT::KeyT key)
        {
            return impl_get<false>(this, key);
        }

        [[nodiscard]] InputT::CPtr get(InputT::KeyT key) const { return impl_get<true>(this, key); }

        [[nodiscard]] Core::IntrusivePtr<InputT> get(const Core::StringAtom& name)
        {
            auto it = _mapping.find(name);
            if (it == _mapping.cend())
            {
                return {};
            }

            return it->second;
        }

        [[nodiscard]] Core::IntrusivePtr<const InputT> get(const Core::StringAtom& name) const
        {
            auto it = _mapping.find(name);
            if (it == _mapping.cend())
            {
                return {};
            }

            return it->second;
        }

        [[nodiscard]] InputT::Ptr create(const Core::StringAtom& name, InputT::KeyT key)
        {
            if (isExist(name))
            {
                warnLog("Catched an attempt to overwrite keybind: {}"_f << name);
                return _mapping[name];
            }
            _mapping.emplace(name, new InputT(name, key));
            debugLog("Mapping was created: '{}'"_f << name);

            return _mapping[name];
        }

        bool remove(const Core::StringAtom& name)
        {
            auto found = _mapping.find(name);
            if (found == _mapping.cend())
            {
                return false;
            }

            debugLog("Mapping was removed: '{}'"_f << found->first);
            _mapping.erase(found);

            return true;
        }

        bool remove(InputT::KeyT key)
        {
            auto found = std::ranges::find(_mapping,
                                           [key](const auto& pair) { return pair.second == key; });

            if (found == _mapping.cend())
            {
                return false;
            }

            return remove(found->first);
        }

        [[nodiscard]] spdlog::logger* getLogger() const override
        {
            return NxSubsystems::getLogger();
        }

        [[nodiscard]] std::filesystem::path getCacheDir() const override { return "Inputs"; }

    private:
        // ==================== PIMPLs =======================
        template<bool isConst, class ThisT>
        [[nodiscard]] static std::conditional_t<isConst, typename InputT::CPtr,
                                                typename InputT::Ptr>
            impl_get(ThisT* self, InputT::KeyT key)
        {
            auto it = std::ranges::find_if(self->_mapping,
                                           [key](const auto& pair) { return key == pair.second; });

            if (it == self->_mapping.cend())
            {
                return {};
            }

            return it->second;
        }

    protected:
        MappingT _mapping;
    };

    class KeyboardInputManger : public InputManger<KeyboardInputAction>
    {
    public:
        [[nodiscard]] Core::StringAtom getCacheHash() const override;
    };

    class MouseInputManger : public InputManger<MouseInputAction>
    {
    public:
        [[nodiscard]] Core::StringAtom getCacheHash() const override;
    };
} // namespace NX
