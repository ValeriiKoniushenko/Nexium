#include "Animations/FrameByFrame/FrameByFrameAnimation.h"

#include "gtest/gtest.h"

using namespace Core;

namespace
{
    Animation::FrameByFrameAnimation makePreview()
    {
        Animation::FrameByFrameAnimation animation;
        animation.addFramesFromSpriteSheet(4, 1);
        animation.setFPS(4.f);
        animation.setLoop(true);
        animation.restart();
        return animation;
    }
} // namespace

TEST(AnimationPreview, UsesAnimationPauseResumeAndRestart)
{
    auto preview = makePreview();
    preview.tick(0.25f);
    EXPECT_EQ(preview.getCurrentFrame(), &preview.getFrames()[1]);
    preview.pause();
    preview.tick(2.f);
    EXPECT_EQ(preview.getCurrentFrame(), &preview.getFrames()[1]);
    preview.resume();
    preview.tick(0.25f);
    EXPECT_EQ(preview.getCurrentFrame(), &preview.getFrames()[2]);
    preview.restart();
    EXPECT_EQ(preview.getCurrentFrame(), &preview.getFrames()[0]);
    EXPECT_TRUE(preview.isPlaying());
}

TEST(AnimationPreview, StopsAtEndAndCanStartAgain)
{
    auto preview = makePreview();
    preview.setLoop(false);
    preview.tick(1.f);
    EXPECT_TRUE(preview.isFinished());
    EXPECT_EQ(preview.getCurrentFrame(), &preview.getFrames()[3]);
    preview.start();
    EXPECT_TRUE(preview.isPlaying());
    EXPECT_EQ(preview.getCurrentFrame(), &preview.getFrames()[0]);
}

TEST(AnimationPreview, UpdatingCopyPreservesPauseAndLeavesDraftUntouched)
{
    auto draft = makePreview();
    draft.stop();
    auto preview = draft;
    preview.restart();
    preview.tick(0.25f);
    preview.pause();
    preview.setFPS(8.f);
    preview.setLoop(false);
    auto edited = draft.getFrames()[1];
    edited.name = "Updated frame"_atom;
    ASSERT_TRUE(preview.setFrame(1, edited));
    ASSERT_TRUE(preview.addFrame("New region"_atom));
    EXPECT_TRUE(preview.isPaused());
    EXPECT_EQ(preview.getCurrentFrame(), &preview.getFrames()[1]);
    EXPECT_EQ(preview.getCurrentFrame()->name, "Updated frame"_atom);
    EXPECT_TRUE(draft.isStopped());
    EXPECT_EQ(draft.getCurrentFrame(), &draft.getFrames()[0]);
    EXPECT_FLOAT_EQ(draft.getFPS(), 4.f);
    EXPECT_TRUE(draft.isLooping());
    EXPECT_EQ(draft.getFramesCount(), 4U);
    preview.resume();
    preview.tick(0.125f);
    EXPECT_EQ(preview.getCurrentFrame(), &preview.getFrames()[2]);
}
