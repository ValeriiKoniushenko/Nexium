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

#include "NxAudioClipEditor.h"

#include "AssetsManager/AssetsManager.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/imgui.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Misc/Configs.h"
#include "Misc/IconsFontAwesome.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

namespace Core
{
    ECS_IMPL(NxAudioClipEditorEWC);

    const char* NxAudioClipEditorEWC::getIcon()
    {
        return ICON_FA_FILE_AUDIO_O;
    }

    void NxAudioClipEditorEWC::updateGuiBasedOnAsset()
    {
        if (_targetAsset)
        {
            _sourcePath = _targetAsset->getFilePath().generic_string();
        }
    }

    void NxAudioClipEditorEWC::onInitialize()
    {
        NxEditorBaseEditorEWC::onInitialize();

        setEnablePreview(true);
        appendFileFilter(std::string("*") + NXAudioClip::AssetT::fileExtension);
    }

    void NxAudioClipEditorEWC::onClose()
    {
        stopPreview();
        _isNewAssetDialogOpen = false;
    }

    void NxAudioClipEditorEWC::onDrawProperties()
    {
        ImGui::TextUnformatted("Source audio file");
        ImGui::SetNextItemWidth(-90.f);
        if (ImGui::InputText("##AudioSourcePath", &_sourcePath) && _targetAsset)
        {
            makeDirty();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FOLDER_OPEN_O " Browse"))
        {
            browseSourceFile();
        }

        ImGui::TextDisabled("WAV, FLAC, or MP3 files inside this project.");
        if (!_targetAsset)
        {
            ImGui::Spacing();
            ImGui::TextDisabled("Choose a source, then use File > New audio clip.");
        }

        drawNewAssetDialog();
    }

    void NxAudioClipEditorEWC::onDrawPreview()
    {
        auto& audioSystem = GetAudioSystem();
        if (_previewVoice.isValid() && !audioSystem.isValid(_previewVoice))
        {
            _previewVoice = {};
        }

        if (!_targetAsset)
        {
            ImGui::TextDisabled("Open or create an audio clip to preview it.");
            return;
        }

        const auto& data = _targetAsset->getData();
        const bool isReady = _targetAsset->isReady();
        ImGui::Text("Status: %s", isReady ? "Ready" : "Unable to load source");
        ImGui::Separator();
        ImGui::Text("Channels: %u", data.channels);
        ImGui::Text("Sample rate: %u Hz", data.sampleRate);
        ImGui::Text("Frames: %llu", static_cast<unsigned long long>(data.frameCount()));
        if (data.sampleRate != 0)
        {
            ImGui::Text("Duration: %.2f s",
                        static_cast<double>(data.frameCount()) / data.sampleRate);
        }

        ImGui::Spacing();
        const bool isPlaying = _previewVoice.isValid();
        if (!isPlaying)
        {
            if (ImGui::Button(ICON_FA_PLAY " Play") && isReady)
            {
                playPreview();
            }
        }
        else if (ImGui::Button(ICON_FA_STOP " Stop"))
        {
            stopPreview();
        }
    }

    void NxAudioClipEditorEWC::onDrawBarExtraFileMenu()
    {
        if (ImGui::MenuItem(ICON_FA_FILE_O " New audio clip"))
        {
            _isNewAssetDialogOpen = true;
        }
    }

    void NxAudioClipEditorEWC::onDiscardChanges()
    {
        stopPreview();
    }

    void NxAudioClipEditorEWC::onSave()
    {
        if (!_targetAsset)
        {
            errorLog("Can't save an audio clip before creating or opening one.");
            return;
        }

        const auto sourcePath = normalizeSourcePath();
        if (!sourcePath)
        {
            return;
        }

        stopPreview();
        _targetAsset->setFilePath(*sourcePath);
        _targetAsset->writeToFile();
        _targetAsset->unloadRequest();
        _targetAsset->loadRequest();
        updateGuiBasedOnAsset();
    }

    bool NxAudioClipEditorEWC::onOpenFromPath(const fs::path& path)
    {
        const auto assetPath = normalizeProjectPath(path);
        if (!assetPath)
        {
            return false;
        }

        stopPreview();
        _targetAsset = GetAssetsManager()->getAudioClip(StringAtom::Intern(assetPath->generic_string()));
        if (!_targetAsset)
        {
            errorLog("Requested audio clip was not found: " + assetPath->generic_string());
            return false;
        }

        return true;
    }

    void NxAudioClipEditorEWC::onFinishOpenFromPath(const fs::path&)
    {
        updateGuiBasedOnAsset();
    }

    void NxAudioClipEditorEWC::drawNewAssetDialog()
    {
        if (_isNewAssetDialogOpen)
        {
            ImGui::OpenPopup("New audio clip");
        }

        if (!ImGui::BeginPopupModal("New audio clip", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            return;
        }

        ImGui::TextUnformatted("Create a metadata asset for the selected source file.");
        ImGui::SetNextItemWidth(430.f);
        ImGui::InputText("Asset path", &_newAssetPath);
        ImGui::TextDisabled("The asset must be located under data/.");

        if (ImGui::Button("Create"))
        {
            createAsset();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            _isNewAssetDialogOpen = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    void NxAudioClipEditorEWC::browseSourceFile()
    {
        const auto selected = AssetsManager::OpenFileSelectionDialog({"*.wav", "*.flac", "*.mp3"});
        if (selected.isEmpty())
        {
            return;
        }

        const auto sourcePath = normalizeProjectPath(selected.toStdStringView());
        if (!sourcePath)
        {
            return;
        }

        _sourcePath = sourcePath->generic_string();
        if (_targetAsset)
        {
            makeDirty();
        }
    }

    void NxAudioClipEditorEWC::createAsset()
    {
        const auto sourcePath = normalizeSourcePath();
        if (!sourcePath)
        {
            return;
        }

        const auto assetPath = normalizeProjectPath(_newAssetPath);
        if (!assetPath || assetPath->extension() != NXAudioClip::AssetT::fileExtension)
        {
            errorLog("New audio clips must use the '.nxaudio' extension.");
            return;
        }

        const auto absolutePath = Config::Path::projectAbsPath / *assetPath;
        std::error_code ec;
        const auto pathInsideData = fs::relative(absolutePath, Config::Path::data, ec);
        if (ec || pathInsideData.empty() || *pathInsideData.begin() == "..")
        {
            errorLog("New audio clips must be created inside the data directory.");
            return;
        }
        if (fs::exists(absolutePath))
        {
            errorLog("Audio clip already exists: " + assetPath->generic_string());
            return;
        }

        fs::create_directories(absolutePath.parent_path(), ec);
        if (ec)
        {
            errorLog("Can't create the audio clip directory: " + ec.message());
            return;
        }

        std::ofstream output(absolutePath);
        if (!output.is_open())
        {
            errorLog("Can't create audio clip metadata: " + absolutePath.generic_string());
            return;
        }

        output << nlohmann::json{{"AssetData", {{"_path", sourcePath->generic_string()}}}}.dump(4);
        output.close();

        GetAssetsManager()->refreshFilesSystem();
        openFromPath(*assetPath);
        _isNewAssetDialogOpen = false;
        ImGui::CloseCurrentPopup();
    }

    void NxAudioClipEditorEWC::playPreview()
    {
        if (!_targetAsset || !_targetAsset->isReady())
        {
            return;
        }

        stopPreview();
        _previewVoice = GetAudioSystem().play(_targetAsset);
    }

    void NxAudioClipEditorEWC::stopPreview()
    {
        if (!_previewVoice.isValid())
        {
            return;
        }

        auto& audioSystem = GetAudioSystem();
        if (audioSystem.isValid(_previewVoice))
        {
            audioSystem.stop(_previewVoice);
        }
        _previewVoice = {};
    }

    std::optional<fs::path> NxAudioClipEditorEWC::normalizeProjectPath(const fs::path& path) const
    {
        if (path.empty())
        {
            errorLog("Path cannot be empty.");
            return std::nullopt;
        }

        const auto absolutePath = path.is_absolute() ? path : Config::Path::projectAbsPath / path;
        std::error_code ec;
        const auto relativePath = fs::relative(absolutePath, Config::Path::projectAbsPath, ec);
        if (ec || relativePath.empty() || *relativePath.begin() == "..")
        {
            errorLog("Path must be located inside the current project: " + path.generic_string());
            return std::nullopt;
        }

        return relativePath.lexically_normal();
    }

    std::optional<fs::path> NxAudioClipEditorEWC::normalizeSourcePath() const
    {
        const auto sourcePath = normalizeProjectPath(_sourcePath);
        if (!sourcePath)
        {
            return std::nullopt;
        }

        const auto extension = sourcePath->extension().generic_string();
        if (extension != ".wav" && extension != ".flac" && extension != ".mp3")
        {
            errorLog("Audio sources must be WAV, FLAC, or MP3 files.");
            return std::nullopt;
        }

        const auto absolutePath = Config::Path::projectAbsPath / *sourcePath;
        if (!fs::is_regular_file(absolutePath))
        {
            errorLog("Audio source file does not exist: " + sourcePath->generic_string());
            return std::nullopt;
        }

        return sourcePath;
    }
} // namespace Core
