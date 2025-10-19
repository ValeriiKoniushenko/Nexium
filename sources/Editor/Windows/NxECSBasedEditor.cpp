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

#include "NxECSBasedEditor.h"

#include "Editor/GuiComponents/Array.h"
#include "Editor/GuiComponents/Combo.h"

using namespace Core::Gui;

namespace Core
{

    using AssimpPostProcessArray =
BaseArray<aiPostProcessSteps,
    decltype([](aiPostProcessSteps data)
    -> Gui::HorizontalLayout::Ptr {
            auto l = Gui::HorizontalLayout::Create();
            const auto combo = l->addChildComponent<Gui::ComboModelBased>();

            combo->setDataProvider(
                [](std::size_t i, StringAtom &out) -> const void * {
                    out = Assimp::aiPostProcessStepsToString(Assimp::aiPostProcessStepsAsVector[i]);
                    return nullptr;
                });
            combo->setSizeProvider(
                [] {
                    return Assimp::aiPostProcessStepsAsVector.size();
                });
            combo->setFlex(Gui::Flex::FlexWidth);

            const auto it = std::ranges::find(Assimp::aiPostProcessStepsAsVector, data);
            if (it != Assimp::aiPostProcessStepsAsVector.end()) {
                combo->setCurrentIndex(it - Assimp::aiPostProcessStepsAsVector.begin());
            }
            return l;
        }),
    decltype([](Gui::HorizontalLayout *layout)
    -> aiPostProcessSteps {
            if (auto modifier = layout->getFirstChildAs<Gui::ComboModelBased>()) {
                return Assimp::aiPostProcessStepsAsVector[modifier->getCurrentIndex()];
            }
            else
            {
                DEBUG_ASSERT(false);
            }

            return static_cast<aiPostProcessSteps>(0);
        })
>;

    ECS_COMPONENT_IMPL(NxECSBasedEditorEWC);
    ECS_COMPONENT_IMPL(ECSEditorMimeAdapter);

    class ActorAdapter : public ECSEditorMimeAdapter
    {
        ECS_COMPONENT_DECL(ActorAdapter, ECSEditorMimeAdapter);

    protected:
        void onApplyAssetData(const nlohmann::json& json) override
        {
            /*if (json.contains("path"))
            {
                _modelInput->setInputtedData(json["path"].get<StringAtom>().toStdString());
            }
            if (json.contains("mainShader"))
            {
                auto str = json["mainShader"].get<StringAtom>();
                _outlineShaderCombo->setCurrentIndex(convertShaderNameToIndex(str));
            }
            if (json.contains("outlineShader"))
            {
                auto str = json["outlineShader"].get<StringAtom>();
                _outlineShaderCombo->setCurrentIndex(convertShaderNameToIndex(str));
            }*/
        }

        void onInitialize() override
        {
            constexpr float labelWidth = 140.0f;
            constexpr float inputWidth = 340.0f;

            _actorLayout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

            _actorPostDraw
                = _actorLayout.addChildComponent<LabelRow<CheckBox>>("Post draw", labelWidth);

            _actorPosition
                = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Position", labelWidth);
            _actorPosition->input->setWidth(inputWidth);
            _actorPosition->input->setFlex(Flex::Fixed);

            _actorRotation
                = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Rotation", labelWidth);
            _actorRotation->input->setWidth(inputWidth);
            _actorRotation->input->setFlex(Flex::Fixed);

            _actorScale
                = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Scale", labelWidth);
            _actorScale->input->setWidth(inputWidth);
            _actorScale->input->setFlex(Flex::Fixed);

            _actorOrigin
                = _actorLayout.addChildComponent<LabelRow<Float3Input>>("Origin", labelWidth);
            _actorOrigin->input->setWidth(inputWidth);
            _actorOrigin->input->setFlex(Flex::Fixed);
        }

        void onDraw(float dt) override
        {
            if (Gui::CollapsingHeader("Actor properties", ImGuiTreeNodeFlags_DefaultOpen))
            {
                _actorLayout.tick(dt);
            }
        }

    protected:
        Gui::VerticalLayout _actorLayout;

        Gui::LabelRow<Gui::TextInput>* _logicalPath = nullptr;
        Gui::LabelRow<Gui::CheckBox>* _actorPostDraw = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorPosition = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorRotation = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorScale = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorOrigin = nullptr;
    };
    ECS_COMPONENT_IMPL(ActorAdapter);

    class StaticMeshBundleAdapter : public ActorAdapter
    {
        ECS_COMPONENT_DECL(StaticMeshBundleAdapter, ActorAdapter);

    protected:
        void onInitialize() override
        {
            ActorAdapter::onInitialize();

            constexpr float defaultLabelWidth = 140.0f;
            constexpr float defaultModifierWidth = 340.0f;

            auto shaderDataProvider = [](std::size_t inputIndex, StringAtom& out) -> const void*
            {
                auto it = GetShaderManager().getShaderMetas().begin();
                std::advance(it, inputIndex);
                if (it == GetShaderManager().getShaderMetas().end())
                {
                    DEBUG_ASSERT(false, "Internal error, can't get specified shader");
                    return nullptr;
                }

                out = it->first;
                return nullptr;
            };
            auto shaderSizeProvider = []
            {
                return GetShaderManager().getShaderMetas().size();
            };

            _layout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

            {
                auto* h = _layout.addChildComponent<HorizontalLayout>();
                h->addChildComponent<Label>("Model path")->setWidth(defaultLabelWidth);
                _modelInput = h->addChildComponent<TextInput>();
                _modelInput->setFlex(Flex::FlexWidth);
                _modelInput->onInput.subscribe(
                    [this](auto)
                    {
                        // makeDirty();
                    });
            }

            {
                auto* h = _layout.addChildComponent<HorizontalLayout>();
                h->setHorizontalAlign(Align::SpaceBetween);
                h->addChildComponent<Label>("Main shader")->setWidth(defaultLabelWidth);
                _mainShaderCombo = h->addChildComponent<ComboModelBased>();
                _mainShaderCombo->setWidth(defaultModifierWidth);
                _mainShaderCombo->setDataProvider(shaderDataProvider);
                _mainShaderCombo->setSizeProvider(shaderSizeProvider);
                _mainShaderCombo->onSelect.subscribe(
                    [this](auto)
                    {
                        // makeDirty();
                    });
            }
            {
                auto* h = _layout.addChildComponent<HorizontalLayout>();
                h->setHorizontalAlign(Align::SpaceBetween);
                h->addChildComponent<Label>("Outline shader")->setWidth(defaultLabelWidth);
                _outlineShaderCombo = h->addChildComponent<ComboModelBased>();
                _outlineShaderCombo->setWidth(defaultModifierWidth);
                _outlineShaderCombo->setDataProvider(shaderDataProvider);
                _outlineShaderCombo->setSizeProvider(shaderSizeProvider);
                _outlineShaderCombo->onSelect.subscribe(
                    [this](auto)
                    {
                        // makeDirty();
                    });
            }
            {
                auto* h = _layout.addChildComponent<HorizontalLayout>();
                h->setHorizontalAlign(Align::SpaceBetween);
                h->addChildComponent<Label>("On load scale")->setWidth(defaultLabelWidth);
                _onLoadScale = h->addChildComponent<FloatInput>();
                _onLoadScale->setWidth(defaultModifierWidth);
                _onLoadScale->setMin(0.0f);
                _onLoadScale->setStep(0.1f);
                _onLoadScale->onInput.subscribe(
                    [this](auto)
                    {
                        // makeDirty();
                    });
            }

            {
                auto* h = _layout.addChildComponent<HorizontalLayout>();
                h->setHorizontalAlign(Align::SpaceBetween);
                h->setVerticalAlign(Align::Top);
                h->addChildComponent<Label>("Post loading flags")->setWidth(defaultLabelWidth);
                _postProcessArray = h->addChildComponent<AssimpPostProcessArray>();
                _postProcessArray->setWidth(defaultModifierWidth);
                _postProcessArray->setFlex(Flex::Fixed);
                _postProcessArray->onChange.subscribe(
                    [this]()
                    {
                        // makeDirty();
                    });
                _postProcessArray->onSave.subscribe(
                    [this](const std::vector<aiPostProcessSteps>& data)
                    {
                        _postProcessFlags = 0;
                        for (auto el : data)
                        {
                            _postProcessFlags |= el;
                        }
                    });
            }
        }

        void onApplyAssetData(const nlohmann::json& json) override
        {
            ActorAdapter::onApplyAssetData(json);

            if (_modelInput && json.contains("path"))
            {
                _modelInput->setInputtedData(json["path"].get<StringAtom>().toStdString());
            }
            if (_outlineShaderCombo && json.contains("mainShader"))
            {
                auto str = json["mainShader"].get<StringAtom>();
                _outlineShaderCombo->setCurrentIndex(convertShaderNameToIndex(str));
            }
            if (_outlineShaderCombo && json.contains("outlineShader"))
            {
                auto str = json["outlineShader"].get<StringAtom>();
                _outlineShaderCombo->setCurrentIndex(convertShaderNameToIndex(str));
            }
        }

        std::size_t convertShaderNameToIndex(const StringAtom& shaderName) const
        {
            const auto it = GetShaderManager().getShaderMetas().find(shaderName);
            return std::distance(GetShaderManager().getShaderMetas().begin(), it);
        }

        StringAtom convertIndexToShaderName(std::size_t index) const
        {
            auto it = GetShaderManager().getShaderMetas().begin();
            std::advance(it, index);
            DEBUG_ASSERT(it != GetShaderManager().getShaderMetas().end());
            return it->first;
        }

        void onDraw(float dt) override
        {
            ActorAdapter::onDraw(dt);

            if (Gui::CollapsingHeader("Static mesh bundle", ImGuiTreeNodeFlags_DefaultOpen))
            {
                _layout.tick(dt);
            }
        }

    protected:
        Gui::VerticalLayout _layout;

        Gui::TextInput* _modelInput = nullptr;
        Gui::ComboModelBased* _mainShaderCombo = nullptr;
        Gui::ComboModelBased* _outlineShaderCombo = nullptr;
        Gui::FloatInput* _onLoadScale = nullptr;
        AssimpPostProcessArray* _postProcessArray = nullptr;
        int _postProcessFlags = 0;
    };
    ECS_COMPONENT_IMPL(StaticMeshBundleAdapter);

    void ECSEditorMimeAdapter::applyAssetData(const nlohmann::json& json)
    {
        if (json.empty())
        {
            globalLog.warnLog("Can't apply asset data. No data.");
            return;
        }

        onApplyAssetData(json);
    }

    void ECSEditorMimeAdapter::draw(float dt)
    {
        onDraw(dt);
    }

    bool NxECSBasedEditorEWC::hasMimeTypeAdapter(const StringAtom& mimeType) const
    {
        return _mimeTypeAdapters.contains(mimeType);
    }

    void NxECSBasedEditorEWC::onInitialize()
    {
        NxEditorBaseEditorEWC::onInitialize();

        registerMimeTypeAdapter<StaticMeshBundleAdapter>(StaticMeshBundle::componentType);

        setEnablePreview(true);

        _headerLayout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        constexpr float labelWidth = 60.0f;
        _logicalPath = _headerLayout.addChildComponent<LabelRow<TextInput>>("ID", labelWidth);
        _logicalPath->input->setFlex(Flex::FlexWidth);
        _logicalPath->input->setReadOnly(true);

        _assetType = _headerLayout.addChildComponent<LabelRow<TextInput>>("Type", labelWidth);
        _assetType->input->setFlex(Flex::FlexWidth);
        _assetType->input->setReadOnly(true);
    }

    void NxECSBasedEditorEWC::onDrawProperties()
    {
        const auto dt = GetWorld().timeDelta;
        _headerLayout.tick(dt);
        for (auto&& child : _children)
        {
            if (auto* typeAdapter = dynamic_cast<ECSEditorMimeAdapter*>(child.get()))
            {
                typeAdapter->draw(dt);
            }
        }
    }

    void NxECSBasedEditorEWC::onDrawPreview()
    {
        NxEditorBaseEditorEWC::onDrawPreview();
    }

    void NxECSBasedEditorEWC::onDiscardChanges()
    {
    }

    void NxECSBasedEditorEWC::onSave()
    {
        if (!_targetAsset)
        {
            return;
        }
    }

    void NxECSBasedEditorEWC::updateGuiBasedOnAsset()
    {
        if (!_targetAsset)
        {
            return;
        }

        _logicalPath->input->setInputtedData(_targetAsset->getLogicPath().toStdString());
        _assetType->input->setInputtedData(_targetAsset->getType().toStdString());
    }

    void NxECSBasedEditorEWC::onOpenFromPath(const std::filesystem::path& path)
    {
        _targetAsset = GetAssetsManager().getAssetByPath(path);
        if (!_targetAsset)
        {
            errorLog("Requested asset not found: " + path.generic_string());
        }
        setup();
    }

    void NxECSBasedEditorEWC::onClose()
    {
        NxEditorBaseEditorEWC::onClose();
        reset();
    }

    ECSEditorMimeAdapter::Ptr NxECSBasedEditorEWC::spawnMimeTypeAdapter(
        const StringAtom& mimeType) const
    {
        if (!hasMimeTypeAdapter(mimeType))
        {
            errorLog("Can't spawn editor's mime adapter. Mime type '{}' is not registered."_f
                     << mimeType);
            return nullptr;
        }

        return _mimeTypeAdapters.at(mimeType)();
    }

    void NxECSBasedEditorEWC::reset()
    {
        removeChildOf<ECSEditorMimeAdapter>();

        _targetAsset.reset();
    }

    void NxECSBasedEditorEWC::setup()
    {
        if (!_targetAsset)
        {
            return;
        }

        if (_targetAsset->getLoadingStatus().cast() != ECSAsset::Status::Loaded)
        {
            DEBUG_ASSERT(false);
            errorLog("Can't setup editor's tree. Asset '{}' is not loaded properly."_f
                     << _targetAsset->getLogicPath());
            return;
        }

        if (hasMimeTypeAdapter(_targetAsset->getType()))
        {
            if (auto adapter = spawnMimeTypeAdapter(_targetAsset->getType()))
            {
                if (auto* p = attachChild(adapter)->castTo<ECSEditorMimeAdapter>())
                {
                    p->applyAssetData(_targetAsset->getAssetData());
                }
            }
        }

        auto data = _targetAsset->getData();
        if (!data)
        {
            errorLog("Can't setup editor's tree. Asset '{}' is null, but expected NOT null."_f
                     << _targetAsset->getLogicPath());
            return;
        }

        int i = 1;
    }

} // namespace Core