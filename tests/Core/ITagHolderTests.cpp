#include "Misc/ITagHolder.h"

#include "gtest/gtest.h"

using namespace Core;

namespace
{

    class UntaggedObject final : public ITagHolder
    {
    };

    class TaggedObject final : public ITagHolder
    {
    public:
        explicit TaggedObject(Tag tags)
            : _tags(tags)
        {
        }

        [[nodiscard]] Tag getTags() const override { return _tags; }

    private:
        Tag _tags;
    };

} // namespace

TEST(ITagHolderTests, DefaultAndOverriddenHoldersExposeTheirDeclaredMasks)
{
    UntaggedObject untagged;
    TaggedObject worldAndEditor(Tag_WorldObject | Tag_EditorInternal);
    const ITagHolder& holder = worldAndEditor;

    ASSERT_EQ(Tag_None, untagged.getTags());
    ASSERT_EQ(Tag_WorldObject | Tag_EditorInternal, holder.getTags());
    ASSERT_TRUE(static_cast<bool>(holder.getTags() & Tag_WorldObject));
    ASSERT_FALSE(static_cast<bool>(holder.getTags() & Tag_GuiObject));
}

TEST(ITagHolderTests, BitwiseOperatorsBuildAndFilterCompositeMasks)
{
    constexpr Tag worldAndGui = Tag_WorldObject | Tag_GuiObject;
    constexpr Tag allKnown
        = Tag_WorldObject | Tag_GuiObject | Tag_EditorInternal | Tag_AnimationController;

    static_assert((worldAndGui & Tag_WorldObject) == Tag_WorldObject);
    static_assert((worldAndGui & Tag_EditorInternal) == Tag_None);

    Tag tags = allKnown;
    tags &= ~Tag_GuiObject;
    ASSERT_EQ(Tag_WorldObject | Tag_EditorInternal | Tag_AnimationController, tags);
    ASSERT_EQ(Tag_None, tags & Tag_GuiObject);
    ASSERT_EQ(Tag_AnimationController, Tag_Any & Tag_AnimationController);
    ASSERT_EQ(Tag_WorldObject ^ Tag_GuiObject, Tag_WorldObject | Tag_GuiObject);
}

TEST(ITagHolderTests, EveryConcreteTagHasAStableStringRoundTrip)
{
    const std::vector<std::pair<Tag, std::string>> tags = {
        { Tag_None, "None" },
        { Tag_WorldObject, "WorldObject" },
        { Tag_GuiObject, "GuiObject" },
        { Tag_EditorInternal, "EditorInternal" },
        { Tag_AnimationController, "AnimationController" },
    };

    for (const auto& [tag, name] : tags)
    {
        ASSERT_EQ(name, TagHelper::ToString(tag));
        const auto parsed = TagHelper::ToTag(name);
        ASSERT_TRUE(parsed.has_value());
        ASSERT_EQ(tag, *parsed);
    }

    ASSERT_EQ("", TagHelper::ToString(Tag_Any));
    ASSERT_FALSE(TagHelper::ToTag("UnknownTag").has_value());
}

TEST(ITagHolderTests, CompositeMasksStringifyInTagDefinitionOrder)
{
    const Tag tags = Tag_WorldObject | Tag_EditorInternal | Tag_AnimationController;

    ASSERT_EQ("WorldObject,EditorInternal,AnimationController", TagHelper::JoinAllToString(tags));
    ASSERT_EQ("WorldObject|EditorInternal|AnimationController",
              TagHelper::JoinAllToString(tags, '|'));
    ASSERT_EQ((std::vector<std::string>{ "WorldObject", "EditorInternal", "AnimationController" }),
              TagHelper::ToStrings(static_cast<uint32_t>(tags)));
    ASSERT_EQ("WorldObject / EditorInternal / AnimationController",
              TagHelper::StringifyToStrings(static_cast<uint32_t>(tags), " / "));
}

TEST(ITagHolderTests, EmptyAndUnknownMasksHavePredictableTextRepresentations)
{
    ASSERT_EQ("None", TagHelper::JoinAllToString(Tag_None));
    ASSERT_EQ((std::vector<std::string>{ "None" }), TagHelper::ToStrings(Tag_None));
    ASSERT_EQ("None", TagHelper::StringifyToStrings(Tag_None));
    ASSERT_EQ("", TagHelper::JoinAllToString(static_cast<Tag>(1u << 31)));
}

TEST(ITagHolderTests, SplitToTagParsesWhitespaceCustomDelimitersAndIgnoresUnknownNames)
{
    ASSERT_EQ(Tag_WorldObject | Tag_GuiObject | Tag_AnimationController,
              TagHelper::SplitToTagFromString(
                  " WorldObject, GuiObject, UnknownTag, AnimationController "));
    ASSERT_EQ(Tag_WorldObject | Tag_EditorInternal,
              TagHelper::SplitToTagFromString("WorldObject|EditorInternal", '|'));
    ASSERT_EQ(Tag_WorldObject, TagHelper::SplitToTagFromString("WorldObject,WorldObject,Missing"));
    ASSERT_EQ(Tag_None, TagHelper::SplitToTagFromString(""));
}
