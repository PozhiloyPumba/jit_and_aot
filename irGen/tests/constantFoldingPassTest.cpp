#include "IRBuilder.hpp"
#include "function.hpp"
#include "passes/constantFoldingPass.hpp"
#include <gtest/gtest.h>
#include "helpers/defines.hpp"


#undef GTEST_GROUP_CUSTOM
#define GTEST_GROUP_CUSTOM ConstantFoldingPassTest

DEFINE_GTEST_CUSTOM(first_test) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *b0 = gen.CreateEmptyBB(g);

    g->SetFirstBB(b0);

    auto *val0 = builder.BuildMovI(uint64_t(7));
    auto *val1 = builder.BuildMovI(uint64_t(8));
    auto *val2 = builder.BuildMovI(uint64_t(42));
    auto *val3 = builder.BuildMovI(uint64_t(5));
    auto *val4 = builder.BuildSub(InstrType::U64, val2, val1);
    auto *val5 = builder.BuildSub(InstrType::U64, val4, val0);
    auto *val6 = builder.BuildSub(InstrType::U64, val5, val3);
    auto *val7 = builder.BuildAddI(val6, uint64_t(10));
    b0->AddInstrBackward(val0);
    b0->AddInstrBackward(val1);
    b0->AddInstrBackward(val2);
    b0->AddInstrBackward(val3);
    b0->AddInstrBackward(val4);
    b0->AddInstrBackward(val5);
    b0->AddInstrBackward(val6);
    b0->AddInstrBackward(val7);

    ConstantFoldingPass p;
    p.Run(g);

    ASSERT_EQ(b0->GetBeginBB(), b0->GetEndBB());
    ASSERT_EQ(static_cast<ImmToValInstr *>(b0->GetBeginBB())->GetImm(), CreateImm(InstrType::U64, 32));

    // g->dump();
}

DEFINE_GTEST_CUSTOM(second_test) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *b0 = gen.CreateEmptyBB(g);

    g->SetFirstBB(b0);

    auto *val0 = builder.BuildMovI(uint64_t(1));
    auto *val1 = builder.BuildAdd(InstrType::U64, val0, val0);
    b0->AddInstrBackward(val0);
    b0->AddInstrBackward(val1);

    ConstantFoldingPass p;
    p.Run(g);

    ASSERT_EQ(b0->GetBeginBB(), b0->GetEndBB());
    ASSERT_EQ(static_cast<ImmToValInstr *>(b0->GetBeginBB())->GetImm(), CreateImm(InstrType::U64, 2));

    // g->dump();
}

DEFINE_GTEST_CUSTOM(shr_test) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *b0 = gen.CreateEmptyBB(g);

    g->SetFirstBB(b0);

	
    auto *val0 = builder.BuildMovI(uint64_t(2));
    auto *val1 = builder.BuildMovI(uint64_t(8));
    auto *val2 = builder.BuildMovI(uint64_t(18));
    auto *val3 = builder.BuildMovI(uint64_t(5));
    auto *val4 = builder.BuildShr(InstrType::U64, val1, val0);
    auto *val5 = builder.BuildAnd(InstrType::U64, val4, val2);
    auto *val6 = builder.BuildShlI(val3, uint64_t(5));
    auto *val7 = builder.BuildAdd(InstrType::U64, val5, val6);

    b0->AddInstrBackward(val0);
    b0->AddInstrBackward(val1);
    b0->AddInstrBackward(val2);
    b0->AddInstrBackward(val3);
    b0->AddInstrBackward(val4);
    b0->AddInstrBackward(val5);
    b0->AddInstrBackward(val6);
    b0->AddInstrBackward(val7);

    ConstantFoldingPass p;
    p.Run(g);

    ASSERT_EQ(b0->GetBeginBB(), b0->GetEndBB());
    ASSERT_EQ(static_cast<ImmToValInstr *>(b0->GetBeginBB())->GetImm(), CreateImm(InstrType::U64, 162));

    // g->dump();
}