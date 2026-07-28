#pragma once

#include "Animations/FrameByFrame/FrameByFrameAnimation.h"

#include <array>
#include <functional>

namespace Core::Animation
{
    class FrameByFrameAnimator;
}

namespace Core
{
    class FrameByFrameAnimationEditor
    {
    public:
        struct PreviewState
        {
            std::size_t frame = 0;
            float timer = 0.f;
        };

        using SaveCallback = std::function<void()>;

        void create(Animation::FrameByFrameAnimator* animator);
        void edit(Animation::FrameByFrameAnimator* animator, const StringAtom& name);
        void draw(float dt, const SaveCallback& onSave);

        static void drawPreview(const Animation::FrameByFrameAnimation& animation, float dt,
                                float size, PreviewState& state);

    private:
        void copyDraftToBuffers();
        void applyBuffersToDraft();
        void drawFrames();
        void drawNamedFrames(const std::vector<StringAtom>& regionNames);
        void drawSpriteSheet();
        [[nodiscard]] bool hasNameConflict() const;

        Animation::FrameByFrameAnimator* _animator = nullptr;
        Animation::FrameByFrameAnimation _draft;
        StringAtom _editedName;
        bool _isOpen = false;
        bool _isCreating = false;

        std::array<char, 128> _name{};
        std::array<char, 256> _atlas{};
        std::array<char, 256> _texture{};
        std::array<char, 256> _frameTexture{};
        float _fps = 10.f;
        bool _loop = true;
        int _sheetColumns = 1;
        int _sheetRows = 1;
        int _sheetSelectedRow = 1;
        PreviewState _preview;
    };
} // namespace Core
