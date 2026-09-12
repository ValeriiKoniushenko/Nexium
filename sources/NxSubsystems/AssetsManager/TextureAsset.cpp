// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "TextureAsset.h"

#include "Foundation/Configs.h"

using namespace RawBackend;
using namespace Foundation;

namespace NX
{
    R_FRIEND_IMPL(TextureAsset);

    void TextureAsset::setFilePath(const std::filesystem::path& value)
    {
        Assert(value.is_relative(), "Path should be relative");
        _path = value;
    }

    void TextureAsset::onLoadRequest()
    {
        _data.release();

        if (_path.empty()) [[unlikely]]
        {
            errorLog("Can't load the texture. The path is empty.");
            return;
        }

        Image img;
        if (!img.loadFromFile(Config::Path::projectAbsPath / _path, _isFlipVertically))
        {
            return;
        }

        _size = img.getSize();
        _channels = img.getChannel();

        _data.loadFromImage(img);
    }

    void TextureAsset::onUnloadRequest()
    {
        _data.release();
    }

} // namespace NX