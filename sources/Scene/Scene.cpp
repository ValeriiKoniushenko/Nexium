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

#include "Scene.h"

namespace SW
{

    void Scene::directDraw()
    {
        for (auto&& mesh : _staticMeshBundles)
        {
            mesh.draw();
        }
    }

    void Scene::setSceneName(Core::StringAtom name)
    {
        if (Verify(!name.isEmpty()))
        {
            _sceneName = name;
        }
    }

    const Core::StringAtom& Scene::getSceneName() const noexcept
    {
        return _sceneName;
    }

    void Scene::addMesh(StaticMeshBundle&& mesh)
    {
        _staticMeshBundles.push_back(std::move(mesh));
        _staticMeshBundles.back().tryReadFromCache();
    }

    void Scene::forceWriteToCacheAllMeshes() const
    {
        for (auto&& mesh : _staticMeshBundles)
        {
            mesh.writeToCache();
        }
    }

    nlohmann::json Scene::toJson() const
    {
        nlohmann::json json;

        json["sceneName"] = _sceneName;
        json["objects"] = nlohmann::json::array();
        for (auto& m : _staticMeshBundles)
        {
            json["objects"].push_back(m.getComponentName());
        }

        return json;
    }

    void Scene::fromJson(const nlohmann::json& json, bool isIgnoreChildren)
    {
        if (json.contains("sceneName"))
        {
            _sceneName = json["sceneName"];
        }

        if (json.contains("objects"))
        {
            // do nothing now
        }
    }

    std::filesystem::path Scene::getCacheDir() const
    {
        return JsonCacheable::getCacheDir() / "scenes";
    }

    Core::StringAtom Scene::getCacheHash() const
    {
        return "RootScene";
    }

    nlohmann::json Scene::toCacheData() const
    {
        forceWriteToCacheAllMeshes();
        return toJson();
    }

    void Scene::fromCacheData(const nlohmann::json& json)
    {
        fromJson(json, false);
    }

    void Scene::tick(float timeDelta)
    {
    }

} // namespace SW
