// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "JustReflectMe/Adapter.h"

namespace Foundation
{

    CLASS();
    class IOutliner
    {
        R_FRIEND(IOutliner);

    public:
        IOutliner(const IOutliner&) = default;
        IOutliner& operator=(const IOutliner&) = default;
        IOutliner(IOutliner&&) noexcept = default;
        IOutliner& operator=(IOutliner&&) noexcept = default;
        virtual ~IOutliner() = default;

        void setIsDrawOutline(bool value) { onOutlineStatusChange(_isDrawOutline = value); }

        void toggleIsDrawOutline() noexcept { setIsDrawOutline(!_isDrawOutline); }
        [[nodiscard]] bool shouldDrawOutline() const noexcept { return _isDrawOutline; }

        friend void swap(IOutliner& a, IOutliner& b) noexcept
        {
            std::swap(a._isDrawOutline, b._isDrawOutline);
        }

    protected:
        IOutliner() = default;

        virtual void onOutlineStatusChange(bool newStatus) = 0;

    private:
        FIELD();
        bool _isDrawOutline = false;
    };
} // namespace Foundation

#include "IOutliner.generated.h" // added by the code generator. Better don't move it.
