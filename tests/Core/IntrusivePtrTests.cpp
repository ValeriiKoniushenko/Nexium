#include "Core/IntrusivePtr.h"

#include "gtest/gtest.h"

using namespace Core;

namespace
{

    class Tracked final : public IntrusiveRefCounter<Tracked>
    {
        INTRUSIVE_PTR_ADAPTERS(Tracked)

    public:
        explicit Tracked(int value = 0)
            : value(value)
        {
        }

        ~Tracked() override { ++destructions; }

        int value = 0;
        int increments = 0;
        int decrements = 0;
        static int destructions;

    protected:
        void onIncrementRef(CounterT) override { ++increments; }
        void onDecrementRef(CounterT) override { ++decrements; }
    };

    int Tracked::destructions = 0;

    class BaseTracked : public IntrusiveRefCounter<BaseTracked>
    {
        INTRUSIVE_PTR_ADAPTERS(BaseTracked)

    public:
        virtual ~BaseTracked() = default;
        virtual int kind() const { return 1; }
    };

    class DerivedTracked final : public BaseTracked
    {
        INTRUSIVE_PTR_ADAPTERS(DerivedTracked)

    public:
        int kind() const override { return 2; }
    };

    class ScopedDestructionReset
    {
    public:
        ScopedDestructionReset() { Tracked::destructions = 0; }
        ~ScopedDestructionReset() { Tracked::destructions = 0; }
    };

} // namespace

TEST(IntrusivePtrTests, EmptyPointersAreNullAndSafe)
{
    IntrusivePtr<Tracked> strong;
    WeakPtr<Tracked> weak;

    ASSERT_FALSE(strong);
    ASSERT_FALSE(strong.isValid());
    ASSERT_EQ(nullptr, strong.get());
    ASSERT_FALSE(weak);
    ASSERT_FALSE(weak.isValid());
    ASSERT_FALSE(weak.hasHardLink());

    auto data = weak.tryLoad();
    ASSERT_FALSE(data.isValid());
    ASSERT_EQ(nullptr, data.get());
}

TEST(IntrusivePtrTests, ConstructionAndDestructionOwnExactlyOneHardReference)
{
    ScopedDestructionReset reset;
    auto* raw = new Tracked(42);

    ASSERT_EQ(0u, raw->getHardRefCount());
    {
        IntrusivePtr<Tracked> strong(raw);
        ASSERT_EQ(raw, strong.get());
        ASSERT_EQ(1u, raw->getHardRefCount());
        ASSERT_EQ(1, raw->increments);
        ASSERT_EQ(0, raw->decrements);
        ASSERT_EQ(42, strong->value);
    }

    ASSERT_EQ(1, Tracked::destructions);
}

TEST(IntrusivePtrTests, CopyAndSelfAssignmentAdjustOnlyTheirOwnReferences)
{
    auto strong = Tracked::Create(7);
    auto* raw = strong.get();

    {
        IntrusivePtr<Tracked> copy = strong;
        ASSERT_EQ(2u, raw->getHardRefCount());

        copy.operator=(copy);
        ASSERT_EQ(2u, raw->getHardRefCount());

        copy.reset();
        ASSERT_EQ(1u, raw->getHardRefCount());
    }

    ASSERT_EQ(1u, raw->getHardRefCount());
}

TEST(IntrusivePtrTests, MoveAndMoveAssignmentTransferWithoutIncrementing)
{
    auto first = Tracked::Create(1);
    auto second = Tracked::Create(2);
    auto* firstRaw = first.get();
    auto* secondRaw = second.get();

    IntrusivePtr<Tracked> moved = std::move(first);
    ASSERT_FALSE(first);
    ASSERT_EQ(firstRaw, moved.get());
    ASSERT_EQ(1u, firstRaw->getHardRefCount());

    moved = std::move(second);
    ASSERT_FALSE(second);
    ASSERT_EQ(secondRaw, moved.get());
    ASSERT_EQ(1u, secondRaw->getHardRefCount());
}

TEST(IntrusivePtrTests, ResetSwapAndDetachPreserveOwnershipAccounting)
{
    auto first = Tracked::Create(1);
    auto second = Tracked::Create(2);
    auto* firstRaw = first.get();
    auto* secondRaw = second.get();

    first.swap(second);
    ASSERT_EQ(secondRaw, first.get());
    ASSERT_EQ(firstRaw, second.get());
    ASSERT_EQ(1u, firstRaw->getHardRefCount());
    ASSERT_EQ(1u, secondRaw->getHardRefCount());

    auto* detached = first.detach();
    ASSERT_FALSE(first);
    ASSERT_EQ(secondRaw, detached);
    ASSERT_EQ(1u, detached->getHardRefCount());

    IntrusivePtr<Tracked> adopted(detached, false);
    ASSERT_EQ(1u, detached->getHardRefCount());
    adopted.reset();
    ASSERT_EQ(1u, firstRaw->getHardRefCount());
}

TEST(IntrusivePtrTests, ConstConversionSharesTheSameObjectAndReferenceCounter)
{
    auto mutablePtr = Tracked::Create(9);
    IntrusivePtr<const Tracked> constPtr = mutablePtr;

    ASSERT_EQ(mutablePtr.get(), constPtr.get());
    ASSERT_EQ(2u, mutablePtr->getHardRefCount());
    ASSERT_EQ(9, constPtr->value);
}

TEST(IntrusivePtrTests, PolymorphicCastsPreserveObjectAndSupportFailure)
{
    auto derived = DerivedTracked::Create();
    BaseTracked::Ptr base = derived;

    auto recovered = DynamicCast<DerivedTracked>(base);
    auto failed = DynamicCast<DerivedTracked>(BaseTracked::Create());

    ASSERT_EQ(derived.get(), recovered.get());
    ASSERT_EQ(2, base->kind());
    ASSERT_FALSE(failed);

    auto staticBase = StaticCast<BaseTracked>(derived);
    ASSERT_EQ(derived.get(), staticBase.get());
}

TEST(IntrusivePtrTests, WeakPointerObservesWithoutKeepingObjectAlive)
{
    ScopedDestructionReset reset;
    auto strong = Tracked::Create(12);
    auto* raw = strong.get();
    WeakPtr<Tracked> weak = strong;

    ASSERT_EQ(1u, raw->getHardRefCount());
    ASSERT_EQ(1u, raw->getWeakRefCount());
    ASSERT_TRUE(weak.hasHardLink());

    {
        auto loaded = weak.tryLoad();
        ASSERT_TRUE(loaded);
        ASSERT_EQ(raw, loaded.get());
        ASSERT_EQ(12, loaded->value);
        ASSERT_EQ(1u, raw->getHardRefCount());
    }

    strong.reset();
    ASSERT_EQ(0, Tracked::destructions);
    ASSERT_FALSE(weak.hasHardLink());
    auto expired = weak.tryLoad();
    ASSERT_EQ(nullptr, expired.get());
    ASSERT_FALSE(weak);
    ASSERT_EQ(1, Tracked::destructions);
}

TEST(IntrusivePtrTests, WeakCopiesMovesAndResetManageWeakReferences)
{
    auto strong = Tracked::Create();
    auto* raw = strong.get();
    WeakPtr<Tracked> first = strong;

    {
        WeakPtr<Tracked> copy = first;
        ASSERT_EQ(2u, raw->getWeakRefCount());

        WeakPtr<Tracked> moved = std::move(copy);
        ASSERT_FALSE(copy);
        ASSERT_EQ(2u, raw->getWeakRefCount());

        moved.reset();
        ASSERT_EQ(1u, raw->getWeakRefCount());
    }

    ASSERT_EQ(1u, raw->getWeakRefCount());
    first.reset();
    ASSERT_EQ(0u, raw->getWeakRefCount());
}

TEST(IntrusivePtrTests, FactoryAdaptersForwardArgumentsAndCreateConstPointers)
{
    auto value = Tracked::Create(33);
    auto constant = Tracked::CreateConst(44);

    ASSERT_EQ(33, value->value);
    ASSERT_EQ(44, constant->value);
    ASSERT_EQ(1u, value->getHardRefCount());
    ASSERT_EQ(1u, constant->getHardRefCount());
}
