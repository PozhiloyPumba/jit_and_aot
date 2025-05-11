#include "IRBuilder.hpp"
#include "function.hpp"
#include "passes/eliminateChecksPass.hpp"
#include <gtest/gtest.h>
#include "helpers/defines.hpp"


#undef GTEST_GROUP_CUSTOM
#define GTEST_GROUP_CUSTOM EliminateChecks_Test

DEFINE_GTEST_CUSTOM(CheckNullEliminationSimple) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *b0 = gen.CreateEmptyBB(g);

	auto* val0 = builder.BuildMovI(10ULL);
	auto* val1 = builder.BuildNullcheck(InstrType::U64, val0);
	auto* val2 = builder.BuildNullcheck(InstrType::U64, val0);

	b0->AddInstrBackward(val0);
	b0->AddInstrBackward(val1);
	b0->AddInstrBackward(val2);

    g->SetFirstBB(b0);
	EliminateChecksPass p;
    p.Run(g);

	ASSERT_EQ(b0->GetBeginBB(), val0);
	ASSERT_EQ(b0->GetEndBB(), val1);

	// g->dump();
}

DEFINE_GTEST_CUSTOM(CheckNullEliminationTwoBBs) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *b0 = gen.CreateEmptyBB(g);
    BB *b1 = gen.CreateEmptyBB(g);

	auto* val0 = builder.BuildMovI(10ULL);
	auto* val1 = builder.BuildNullcheck(InstrType::U64, val0);
	auto* val2 = builder.BuildNullcheck(InstrType::U64, val0);
    b0->AddInstrBackward(val0);
    b0->AddInstrBackward(val1);
    b0->AddInstrBackward(val2);

	auto* val3 = builder.BuildMovI(42ULL);
	auto* val4 = builder.BuildNullcheck(InstrType::U64, val0);
    b1->AddInstrBackward(val3);
    b1->AddInstrBackward(val4);

	g->AddBBAsSucc(b1, b0, false);

    g->SetFirstBB(b0);
	EliminateChecksPass p;
    p.Run(g);
	ASSERT_EQ(b1->GetBeginBB(), b1->GetEndBB());

	// g->dump();
}

DEFINE_GTEST_CUSTOM(CheckNullEliminationDiamondBBs) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

	auto *b0 = gen.CreateEmptyBB(g);
	auto *b1 = gen.CreateEmptyBB(g);
	auto *b2 = gen.CreateEmptyBB(g);
	auto *b3 = gen.CreateEmptyBB(g);
  
	auto* val0 = builder.BuildMovI(10ULL);
	auto* val1 = builder.BuildNullcheck(InstrType::U64, val0);
	auto* val2 = builder.BuildNullcheck(InstrType::U64, val0);
	b0->AddInstrBackward(val0);
	b0->AddInstrBackward(val1);
	b0->AddInstrBackward(val2);

	auto* val3 = builder.BuildMovI(42ULL);
	auto* val4 = builder.BuildNullcheck(InstrType::U64, val0);
	b1->AddInstrBackward(val3);
	b1->AddInstrBackward(val4);

	auto* val5 = builder.BuildNullcheck(InstrType::U64, val0);
	auto* val6 = builder.BuildMovI(13ULL);
	auto* val7 = builder.BuildNullcheck(InstrType::U64, val0);
	b2->AddInstrBackward(val5);
	b2->AddInstrBackward(val6);
	b2->AddInstrBackward(val7);

	auto* val8 = builder.BuildMovI(0ULL);
	auto* val9 = builder.BuildNullcheck(InstrType::U64, val0);
	b3->AddInstrBackward(val8);
	b3->AddInstrBackward(val9);

    g->AddBBAsSucc(b1, b0, false);
    g->AddBBAsSucc(b2, b0, true);
    g->AddBBAsSucc(b3, b1, false);
    g->AddBBAsSucc(b3, b2, false);
	
    g->SetFirstBB(b0);
	EliminateChecksPass p;
    p.Run(g);

	ASSERT_EQ(b0->GetBeginBB(), val0);
	ASSERT_EQ(b0->GetEndBB(), val1);
	ASSERT_EQ(b1->GetBeginBB(), b1->GetEndBB());
	ASSERT_EQ(b2->GetBeginBB(), b2->GetEndBB());
	ASSERT_EQ(b3->GetBeginBB(), b3->GetEndBB());
}

DEFINE_GTEST_CUSTOM(CheckBoundsEliminationSimple) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

	auto* b0 = gen.CreateEmptyBB(g);

	auto* val0 = builder.BuildMovI(10ULL);
	auto* val1 = builder.BuildBoundsCheck(val0, 8);
	auto* val2 = builder.BuildBoundsCheck(val0, 8);
	auto* val3 = builder.BuildBoundsCheck(val0, 9);
	b0->AddInstrBackward(val0);
	b0->AddInstrBackward(val1);
	b0->AddInstrBackward(val2);
	b0->AddInstrBackward(val3);

    g->SetFirstBB(b0);
	EliminateChecksPass p;
    p.Run(g);

	ASSERT_EQ(b0->GetBeginBB(), val0);
	ASSERT_EQ(b0->GetEndBB(), val1);
}

DEFINE_GTEST_CUSTOM(CheckBoundsEliminationSmaller) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

	auto *b0 = gen.CreateEmptyBB(g);
  
	auto* val0 = builder.BuildMovI(10ULL);
	auto* val1 = builder.BuildBoundsCheck(val0, 8);
	auto* val2 = builder.BuildBoundsCheck(val0, 7);
	b0->AddInstrBackward(val0);
	b0->AddInstrBackward(val1);
	b0->AddInstrBackward(val2);

    g->SetFirstBB(b0);
	EliminateChecksPass p;
    p.Run(g);

	ASSERT_EQ(b0->GetBeginBB(), val0);
	ASSERT_EQ(b0->GetEndBB(), val2);
}
  
DEFINE_GTEST_CUSTOM(CheckBoundsEliminationDiamondBBs) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

	auto* b0 = gen.CreateEmptyBB(g);
	auto* b1 = gen.CreateEmptyBB(g);
	auto* b2 = gen.CreateEmptyBB(g);
	auto* b3 = gen.CreateEmptyBB(g);
	auto* b4 = gen.CreateEmptyBB(g);
  
	auto* val0 = builder.BuildMovI(10ULL);
	auto* val1 = builder.BuildBoundsCheck(val0, 100);
	auto* val2 = builder.BuildBoundsCheck(val0, 8);
	b0->AddInstrBackward(val0);
	b0->AddInstrBackward(val1);
	b0->AddInstrBackward(val2);

	auto* val3 = builder.BuildMovI(42ULL);
	auto* val4 = builder.BuildBoundsCheck(val0, 44);
	b1->AddInstrBackward(val3);
	b1->AddInstrBackward(val4);

	auto* val5 = builder.BuildBoundsCheck(val0, 99);
	auto* val6 = builder.BuildMovI(13ULL);
	auto* val7 = builder.BuildBoundsCheck(val0, 99);
	b2->AddInstrBackward(val5);
	b2->AddInstrBackward(val6);
	b2->AddInstrBackward(val7);

	auto* val8 = builder.BuildMovI(0ULL);
	auto* val9 = builder.BuildBoundsCheck(val0, 0);
	b3->AddInstrBackward(val8);
	b3->AddInstrBackward(val9);

	auto* val10 = builder.BuildBoundsCheck(val8, 101);
	b4->AddInstrBackward(val10);

	g->AddBBAsSucc(b1, b0, false);
    g->AddBBAsSucc(b2, b0, true);
    g->AddBBAsSucc(b3, b1, false);
    g->AddBBAsSucc(b3, b2, false);
    g->AddBBAsSucc(b4, b3, false);

    g->SetFirstBB(b0);
	EliminateChecksPass p;
    p.Run(g);

	ASSERT_EQ(b1->GetBeginBB(), b1->GetEndBB());
	ASSERT_EQ(b2->GetBeginBB(), b2->GetEndBB());
	ASSERT_EQ(b3->GetBeginBB(), val8);
	ASSERT_EQ(b3->GetEndBB(), val9);
	ASSERT_EQ(b4->GetBeginBB(), val10);
	ASSERT_EQ(b4->GetBeginBB(), b4->GetEndBB());
}
