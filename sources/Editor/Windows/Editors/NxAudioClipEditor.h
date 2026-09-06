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

#pragma once

#include "AssetsManager/AudioClipAsset.h"
#include "Editor/Windows/Editors/NxEditorBaseEditor.h"

#include <optional>
#include <string>

namespace Core
{
    /// @brief Editor window for creating, validating, and previewing `.nxaudio` metadata assets.
    /// Preview voices are always stopped when the target changes or the window closes.
    CLASS();
    class NxAudioClipEditorEWC : public NxEditorBaseEditorEWC
    {
        ECS_DECL(NxAudioClipEditorEWC, Core::NxEditorBaseEditorEWC);

    public:
        /// @brief Get the asset-browser icon for an audio clip.
        /// @return Icon identifier understood by the editor UI.
        [[nodiscard]] const char* getIcon() override;

    protected:
        void updateGuiBasedOnAsset() override;
        void onInitialize() override;
        void onClose() override;
        void onDrawProperties() override;
        void onDrawPreview() override;
        void onDrawBarExtraFileMenu() override;
        void onDiscardChanges() override;
        void onSave() override;
        bool onOpenFromPath(const std::filesystem::path& path) override;
        void onFinishOpenFromPath(const std::filesystem::path& path) override;

    private:
        void drawNewAssetDialog();
        void browseSourceFile();
        void createAsset();
        void playPreview();
        void stopPreview();

        [[nodiscard]] std::optional<std::filesystem::path> normalizeProjectPath(
            const std::filesystem::path& path) const;
        [[nodiscard]] std::optional<std::filesystem::path> normalizeSourcePath() const;

    private:
        NXAudioClip _targetAsset;
        Audio::VoiceHandle _previewVoice;
        std::string _sourcePath;
        std::string _newAssetPath = "data/assets/audio/NewAudio.nxaudio";
        bool _isNewAssetDialogOpen = false;
    };
} // namespace Core

#include "NxAudioClipEditor.generated.h" // added by the code generator. Better don't move it.
