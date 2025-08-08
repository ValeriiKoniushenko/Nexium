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
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "InputDevices/ModuleInfo.h"

#include <vector>

namespace SW
{
    class Scene
    {
    public:
        Scene() = default;
        virtual ~Scene();
        void directDraw();

        void setSceneName(Core::StringAtom name);
        [[nodiscard]] const Core::StringAtom& getSceneName() const noexcept;

        void addMesh(StaticMeshBundle&& mesh);
        [[nodiscard]] const std::vector<StaticMeshBundle>& getStaticMeshBundles() const noexcept
        {
            return _staticMeshBundles;
        }
        [[nodiscard]] std::vector<StaticMeshBundle>& getStaticMeshBundles() noexcept
        {
            return _staticMeshBundles;
        }

    protected:
        void forceWriteToCacheAllMeshes();

    protected:
        std::vector<StaticMeshBundle> _staticMeshBundles;
        Core::StringAtom _sceneName = "None";
    };
} // namespace SW