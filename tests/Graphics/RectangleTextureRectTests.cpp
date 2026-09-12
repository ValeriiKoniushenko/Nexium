#include "Scene/Rectangle.h"

#include "gtest/gtest.h"
#include <cmath>

namespace
{
    class TextureRectangle : public Core::SceneObj::RectangleAnimated
    {
    public:
        std::optional<Core::FRect> alphaBounds = Core::FRect(0.1f, 0.8f, 0.4f, 0.2f);
        [[nodiscard]] std::optional<Core::FRect> getLocalTextureRect() const override
        {
            return toLocalTextureRect(alphaBounds);
        }
    };

    void ExpectBounds(const std::optional<Core::FRect>& rect, float left, float top, float right,
                      float bottom)
    {
        if (!rect)
        {
            ADD_FAILURE() << "Expected texture bounds";
            return;
        }
        EXPECT_NEAR(rect->getLeftTop().x, left, 0.001f);
        EXPECT_NEAR(rect->getLeftTop().y, top, 0.001f);
        EXPECT_NEAR(rect->getRightBottom().x, right, 0.001f);
        EXPECT_NEAR(rect->getRightBottom().y, bottom, 0.001f);
        EXPECT_TRUE(rect->isValid());
    }
} // namespace

TEST(RectangleTextureRect, LocalBoundsExcludeEveryObjectTransform)
{
    TextureRectangle rectangle;
    rectangle.setPosition({ 100.f, 200.f, 0.f });
    rectangle.setScale({ -2.f, 3.f, 1.f });
    rectangle.setRotationZ(37.f);
    rectangle.setOrigin({ 5.f, 10.f, 0.f });
    ExpectBounds(rectangle.getLocalTextureRect(), 10.f, 80.f, 40.f, 20.f);
}

TEST(RectangleTextureRect, GlobalBoundsApplyScaleOnceAndUseCurrentProperties)
{
    TextureRectangle rectangle;
    rectangle.setPosition({ 100.f, 200.f, 0.f });
    rectangle.setScale({ 2.f, 3.f, 1.f });
    ExpectBounds(rectangle.getGlobalTextureRect(), 120.f, 440.f, 180.f, 260.f);
    rectangle.setPosition({ -10.f, -20.f, 0.f });
    ExpectBounds(rectangle.getGlobalTextureRect(), 10.f, 220.f, 70.f, 40.f);
}

TEST(RectangleTextureRect, GlobalBoundsTransformAllCornersWithMirroringAndRotation)
{
    TextureRectangle rectangle;
    rectangle.setScale({ -2.f, 1.f, 1.f });
    rectangle.setRotationZ(90.f);
    rectangle.setOrigin({ 5.f, 10.f, 0.f });
    // Local (10..40,20..80), origin -> (5..35,10..70), scale -> (-70..-10,10..70).
    ExpectBounds(rectangle.getGlobalTextureRect(), -70.f, -10.f, -10.f, -70.f);
}

TEST(RectangleTextureRect, GlobalBoundsIncludeParentAndGrandparent)
{
    Core::SceneObject root;
    root.setPosition({ 100.f, 200.f, 0.f });
    root.setRotationZ(90.f);
    auto* parent = root.addChildComponent<Core::SceneObject>();
    parent->setScale({ 2.f, 3.f, 1.f });
    auto* child = parent->addChildComponent<TextureRectangle>();
    child->setPosition({ 5.f, 10.f, 0.f });
    ExpectBounds(child->getGlobalTextureRect(), -170.f, 290.f, 10.f, 230.f);
    root.setPosition({ 110.f, 220.f, 0.f });
    ExpectBounds(child->getGlobalTextureRect(), -160.f, 310.f, 20.f, 250.f);
}

TEST(RectangleTextureRect, TransparentTextureHasNeitherLocalNorGlobalBounds)
{
    TextureRectangle rectangle;
    rectangle.alphaBounds.reset();
    EXPECT_FALSE(rectangle.getLocalTextureRect());
    EXPECT_FALSE(rectangle.getGlobalTextureRect());
}

TEST(RectangleTextureRect, RotatedGlobalBoundsEncloseAllFourCorners)
{
    TextureRectangle rectangle;
    rectangle.setRotationZ(45.f);
    const float diagonal = std::sqrt(0.5f);
    ExpectBounds(rectangle.getGlobalTextureRect(), -70.f * diagonal, 120.f * diagonal,
                 20.f * diagonal, 30.f * diagonal);
}
