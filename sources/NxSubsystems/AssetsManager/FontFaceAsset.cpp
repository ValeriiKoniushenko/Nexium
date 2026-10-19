// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "FontFaceAsset.h"

#include "Foundation/Configs.h"

namespace NX
{
    R_FRIEND_IMPL(FontFaceAsset);

    void FontFaceAsset::setFilePath(const std::filesystem::path& value)
    {
        Assert(value.is_relative(), "Path should be relative");
        _path = value;
    }

    void FontFaceAsset::onLoadRequest()
    {
        _data.release();

        if (_path.empty()) [[unlikely]]
        {
            errorLog("Can't load the font face. The path is empty.");
            return;
        }

        _data.setSourcePath(Foundation::Config::Path::projectAbsPath / _path);
        if (!_data.generateFace(0))
        {
            errorLog("Can't generate the font face from '{}'"_f << _path.generic_string());
            _data.release();
        }
    }

    void FontFaceAsset::onUnloadRequest()
    {
        _data.release();
    }

} // namespace NX
