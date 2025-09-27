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

#include "InputAction.h"
#include "ModuleInfo.h"

#include <unordered_map>

namespace Core
{
    template<IsInputAction InputTParam>
    class InputManger : public BaseLog
    {
    public:
        using Self = InputManger;
        template<bool isConst>
        using AdaptiveRawPtr = std::conditional_t<isConst, const InputManger, InputManger>*;
        using InputT = InputTParam;
        using MappingT = std::unordered_map<StringAtom, typename InputT::Ptr>;

        InputManger() = default;

        ~InputManger() override = default;

        virtual void update()
        {
            for (auto&& [_, ia] : _mapping)
            {
                ia->update();
            }
        }

        [[nodiscard]] const MappingT& getMapping() const noexcept { return _mapping; }

        [[nodiscard]] InputT::Ptr getOrCreate(const StringAtom& name, InputT::KeyT key)
        {
            if (isExist(name))
            {
                return _mapping[name];
            }

            return create(name, key);
        }

        [[nodiscard]] bool isExist(InputT::KeyT key) const { return !!impl_get<true>(this, key); }

        [[nodiscard]] bool isExist(const StringAtom& name) const
        {
            return !!impl_get<true>(this, name);
        }

        [[nodiscard]] typename InputT::Ptr get(typename InputT::KeyT key)
        {
            return impl_get<false>(this, key);
        }

        [[nodiscard]] InputT::CPtr get(InputT::KeyT key) const { return impl_get<true>(this, key); }

        [[nodiscard]] InputT::Ptr get(const StringAtom& name)
        {
            return impl_get<false>(this, name);
        }

        [[nodiscard]] InputT::CPtr get(const StringAtom& name) const
        {
            return impl_get<true>(this, name);
        }

        [[nodiscard]] InputT::Ptr create(const StringAtom& name, InputT::KeyT key)
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

        bool remove(const StringAtom& name)
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
                                           [key](const auto& pair)
                                           {
                                               return pair.second == key;
                                           });

            if (found == _mapping.cend())
            {
                return false;
            }

            return remove(found->first);
        }

        [[nodiscard]] spdlog::logger* getLogger() const override
        {
            return InputDevices::getLogger();
        }

    private:
        // ==================== PIMPLs =======================
        template<bool isConst>
        [[nodiscard]] static std::conditional_t<isConst, typename InputT::CPtr,
                                                typename InputT::Ptr>
            impl_get(AdaptiveRawPtr<isConst> self, InputT::KeyT key)
        {
            auto it = std::ranges::find_if(self->_mapping,
                                           [key](const auto& pair)
                                           {
                                               return key == pair.second;
                                           });

            if (it == self->_mapping.cend())
            {
                return {};
            }

            return it->second;
        }

        template<bool isConst>
        [[nodiscard]] static std::conditional_t<isConst, typename InputT::CPtr,
                                                typename InputT::Ptr>
            impl_get(AdaptiveRawPtr<isConst> self, const StringAtom& name)
        {
            auto it = self->_mapping.find(name);
            if (it == self->_mapping.cend())
            {
                return {};
            }

            return it->second;
        }

    protected:
        MappingT _mapping;
    };

    class KeyboardInputManger : public InputManger<KeyboardInputAction>, public JsonAdapter
    {
    public:
        [[nodiscard]] nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& json, bool isIgnoreChildren) override;
    };

    class MouseInputManger : public InputManger<MouseInputAction>
    {
    };
} // namespace Core