/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

    ECS_IMPL(SceneObject);

    spdlog::logger* Core::SceneObject::getLogger() const
    {
        return ::Scene::getLogger();
    }

    SceneState SceneObject::getSceneState() const
    {
        auto&& name = getComponentName().toStdString();
        return { .name = (name.empty() ? "None" : name),
                 .trans = static_cast<const Transformable&>(*this),
                 .assetType = getComponentType(),
                 .referenceAsset = _referencedAsset,
                 .typeSpecificData = getTypeSpecificSceneDataAsJson() };
    }

    Tag SceneObject::getTags() const
    {
        return Tag_WorldObject;
    }

    StringAtom SceneObject::shortStringify() const
    {
        auto name = getComponentName();
        if (name.isEmpty())
        {
            return "<no name>"_atom;
        }

        return "{} [{}]"_f << name << getComponentType();
    }

    void SceneObject::recalculateMatrices(const glm::mat4& mat)
    {
        Transformable::recalculateMatrices(mat);

        for (auto&& comp : _children)
        {
            if (auto* trans = dynamic_cast<Transformable*>(comp.get()))
            {
                trans->recalculateMatrices(_cachedModelMatrix);
            }
        }
    }

    void SceneObject::onPreDeserialize(AbstractComponent* obj)
    {
        BaseComponent::onPreDeserialize(obj);
    }

    void SceneObject::onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs)
    {
        BaseComponent::onPostDeserialize(obj, logs);
        Transformable::onPostDeserialize(dynamic_cast<Transformable*>(obj), logs);
    }

    void SceneObject::onPreSerialize(const AbstractComponent* obj) const
    {
        BaseComponent::onPreSerialize(obj);
    }

    void SceneObject::onPostSerialize(const AbstractComponent* obj,
                                      const RLogsCollector& logs) const
    {
        BaseComponent::onPostSerialize(obj, logs);
    }

    bool SceneObject::hasReferencedAsset() const noexcept
    {
        return !_referencedAsset.isEmpty();
    }

    StringAtom SceneObject::getReferencedAsset() const
    {
#if defined(DEBUG)
        Assert(_referencedAsset.isStatic());
#endif
        return _referencedAsset;
    }

    void SceneObject::_setReferencedAsset(const StringAtom& logicPath)
    {
        _referencedAsset = logicPath;

#if defined(DEBUG)
        Assert(_referencedAsset.isStatic());
#endif
    }

    nlohmann::json SceneObject::getTypeSpecificSceneDataAsJson() const
    {
        return {};
    }

    void SceneObject::applyTypeSpecificSceneData(const nlohmann::json& data)
    {
    }

    void SceneObject::makeTransformableTreeDirty()
    {
        forEach(
            [](BaseComponent* obj)
            {
                if (auto* t = dynamic_cast<Transformable*>(obj))
                {
                    t->setDirtyMatrices();
                }
            });
    }

    void SceneObject::onInitialize()
    {
        BaseComponent::onInitialize();
    }

    void SceneObject::onPreInitialize()
    {
        BaseComponent::onPreInitialize();
    }

    void SceneObject::onOutlineStatusChange(bool newStatus)
    {
    }

    void SceneObject::onDirtyMatrix()
    {
        Transformable::onDirtyMatrix();
        makeTransformableTreeDirty();
    }

    void to_json(nlohmann::json& j, const SceneState& v)
    {
        j["name"] = v.name;
        j["trans"] = R<Transformable>::Serialize(v.trans).getData();
        j["assetType"] = v.assetType;
        j["referenceAsset"] = v.referenceAsset;
        j["typeSpecificData"] = v.typeSpecificData;
    }

    void from_json(const nlohmann::json& j, SceneState& v)
    {
        v.name = j.at("name").get<std::string>();
        R<Transformable>::Deserialize({ j.at("trans") }, v.trans);
        v.assetType = j.at("assetType").get<StringAtom>();
        v.referenceAsset = StringAtom::Intern(j.at("referenceAsset").get<StringAtom>());
        if (j.contains("typeSpecificData"))
        {
            v.typeSpecificData = j.at("typeSpecificData");
        }
    }

} // namespace Core
