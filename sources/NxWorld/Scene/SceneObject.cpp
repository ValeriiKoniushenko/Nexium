// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SceneObject.h"

#include "Scene/PrivateModuleInfo.h"

namespace Core
{

    ECS_IMPL(SceneObject);

    spdlog::logger* Core::SceneObject::getLogger() const
    {
        return NxWorld::getLogger();
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
#if defined(NEXIUM_DEBUG)
        Assert(_referencedAsset.isStatic());
#endif
        return _referencedAsset;
    }

    void SceneObject::_setReferencedAsset(const StringAtom& logicPath)
    {
        _referencedAsset = logicPath;

#if defined(NEXIUM_DEBUG)
        Assert(_referencedAsset.isStatic());
#endif
    }

    nlohmann::json SceneObject::getTypeSpecificSceneDataAsJson() const
    {
        nlohmann::json out;

        out["_isEnabled"] = _isEnabled;

        return out;
    }

    void SceneObject::applyTypeSpecificSceneData(const nlohmann::json& data)
    {
        if (data.contains("_isEnabled"))
        {
            _isEnabled = data.value("_isEnabled", true);
        }
    }

    void SceneObject::draw(BaseCamera& camera)
    {
        IDrawable::draw(camera);

        if (!isEnabled())
        {
            return;
        }

        onDraw(camera);
    }

    glm::vec3 SceneObject::getGlobalPosition() const
    {
        auto out = glm::vec3(0);

        const BaseComponent* p = this;
        while (p)
        {
            if (auto* t = dynamic_cast<const Transformable*>(p))
            {
                out += t->getPosition();
            }
            p = p->getParent();
        }

        return out;
    }

    glm::vec3 SceneObject::getGlobalRotation() const
    {
        auto out = glm::vec3(0);

        const BaseComponent* p = this;
        while (p)
        {
            if (auto* t = dynamic_cast<const Transformable*>(p))
            {
                out += t->getRotation();
            }
            p = p->getParent();
        }

        return out;
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
