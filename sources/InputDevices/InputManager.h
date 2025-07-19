// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "InputAction.h"

#include <unordered_map>

namespace SW
{

    template<IsInputAction _InputT>
    class InputManger final
    {
    public:
        using Self = InputManger;
        template<bool isConst>
        using AdaptiveRawPtr = std::conditional_t<isConst, const InputManger, InputManger>*;
        using InputT = _InputT;

        InputManger() = default;
        ~InputManger() = default;

        void update()
        {
            for (auto&& [_, ia] : _mapping)
            {
                ia->update();
            }
        }

        [[nodiscard]] typename InputT::Ptr getOrCreate(const Core::StringAtom& name,
                                                       typename InputT::KeyT key)
        {
            if (isExist(key))
            {
                return _mapping[key];
            }

            return create(name, key);
        }

        [[nodiscard]] bool isExist(typename InputT::KeyT key) const
        {
            return !!impl_get<true>(this, key);
        }
        [[nodiscard]] bool isExist(const Core::StringAtom& name) const
        {
            return !!impl_get<true>(this, name);
        }

        [[nodiscard]] typename InputT::Ptr get(typename InputT::KeyT key)
        {
            return impl_get<false>(this, key);
        }

        [[nodiscard]] typename InputT::CPtr get(typename InputT::KeyT key) const
        {
            return impl_get<true>(this, key);
        }

        [[nodiscard]] typename InputT::Ptr get(const Core::StringAtom& name)
        {
            return impl_get<false>(this, name);
        }

        [[nodiscard]] typename InputT::CPtr get(const Core::StringAtom& name) const
        {
            return impl_get<true>(this, name);
        }

        [[nodiscard]] typename InputT::Ptr create(const Core::StringAtom& name,
                                                  typename InputT::KeyT key)
        {
            _mapping.emplace(key, new InputT(name, key));
            _nameMapping.emplace(name, _mapping[key].get());
            return _mapping[key];
        }

    private:
        // ==================== PIMPLs =======================
        template<bool isConst>
        [[nodiscard]] static std::conditional_t<isConst, typename InputT::CPtr,
                                                typename InputT::Ptr>
            impl_get(AdaptiveRawPtr<isConst> self, typename InputT::KeyT key)
        {
            auto it = self->_mapping.find(key);
            if (it == self->_mapping.cend())
            {
                return {};
            }

            return it->second;
        }

        template<bool isConst>
        [[nodiscard]] static std::conditional_t<isConst, typename InputT::CPtr,
                                                typename InputT::Ptr>
            impl_get(AdaptiveRawPtr<isConst> self, const Core::StringAtom& name)
        {
            auto it = self->_nameMapping.find(name);
            if (it == self->_nameMapping.cend())
            {
                return {};
            }

            const auto key = it->second->getKey();
            if (!key)
            {
                return {};
            }

            auto found = self->_mapping.find(key.value());
            return found != self->_mapping.cend() ? found->second : nullptr;
        }

    private:
        std::unordered_map<typename InputT::KeyT, typename InputT::Ptr> _mapping;
        std::unordered_map<Core::StringAtom, InputT*> _nameMapping;
    };

    using KeyboardInputManger = InputManger<KeyboardInputAction>;

} // namespace SW