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

#include "SceneObject.h"

#include "Scene/ModuleInfo.h"

namespace Core
{

    ECS_COMPONENT_IMPL(SceneObject);
    R_FRIEND_IMPL(SceneObject);

    spdlog::logger* Core::SceneObject::getLogger() const
    {
        return ::Scene::getLogger();
    }

    SceneState SceneObject::getSceneState() const
    {
        return { .name = getComponentName().toStdString(),
                 .trans = static_cast<const Transformable&>(*this),
                 .assetType = getComponentType() };
    }

    void SceneObject::onOutlineStatusChange(bool newStatus)
    {
    }

    void to_json(nlohmann::json& j, const SceneState& v)
    {
        j["name"] = v.name;
        j["trans"] = R<Transformable>::Serialize(v.trans).getData();
        j["assetType"] = v.assetType;
    }

    void from_json(const nlohmann::json& j, SceneState& v)
    {
        v.name = j.at("name").get<std::string>();
        R<Transformable>::Deserialize({ j.at("trans") }, v.trans);
        v.assetType = j.at("assetType").get<StringAtom>();
    }

} // namespace Core