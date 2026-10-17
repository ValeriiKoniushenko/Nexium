// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Font.h"

#include "NxSubsystems/PrivateModuleInfo.h"
#include "RawBackend/Fonts/FreeTypeLibrary.h"

namespace NX
{
    Face::Face()
        : _face{ std::make_unique<RawBackend::Font::Face>() }
    {
    }

    Face::Face(Face&& other) noexcept
    {
        release();
        swap(*this, other);
    }

    Face& Face::operator=(Face&& other) noexcept
    {
        if (&other == this) [[unlikely]]
        {
            return *this;
        }

        Face tmp(std::move(*this));
        swap(other, *this);

        return *this;
    }

    void Face::setSourcePath(const std::filesystem::path& path)
    {
        _face->setSourcePath(path);
    }

    std::size_t Face::getFacesNumber()
    {
        return _face->getFacesNumber();
    }

    bool Face::generateFace(std::size_t faceIndex)
    {
        return _face->generateFace(static_cast<FT_Long>(faceIndex));
    }

    bool Face::setPixelSize(Core::UISize2 size)
    {
        return _face->setPixelSize(size);
    }

    bool Face::setPixelSize(std::size_t height)
    {
        return _face->setPixelSize(static_cast<unsigned int>(height));
    }

    void Face::release()
    {
        _face->release();
    }

    spdlog::logger* Face::getLogger() const
    {
        return NxSubsystems::getLogger();
    }

} // namespace NX
