#include "livenessAnalysis.hpp"
#include "IRBuilder.hpp"
#include "function.hpp"
#include <gtest/gtest.h>

TEST(LinOrder_Test, LinOrderTestFirst) {
    using namespace IRGen;

    InstructionBuilder builder;
    IRGenerator gen;

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *A = gen.CreateEmptyBB(g);
    BB *B = gen.CreateEmptyBB(g);
    BB *C = gen.CreateEmptyBB(g);
    BB *D = gen.CreateEmptyBB(g);
    BB *E = gen.CreateEmptyBB(g);
    BB *F = gen.CreateEmptyBB(g);
    BB *G = gen.CreateEmptyBB(g);
    BB *H = gen.CreateEmptyBB(g);
    BB *I = gen.CreateEmptyBB(g);
    BB *J = gen.CreateEmptyBB(g);
    BB *K = gen.CreateEmptyBB(g);
    BB *L = gen.CreateEmptyBB(g);
    BB *M = gen.CreateEmptyBB(g);
    BB *N = gen.CreateEmptyBB(g);
    BB *O = gen.CreateEmptyBB(g);
    BB *P = gen.CreateEmptyBB(g);

    g->AddBBAsSucc(B, A, true);
    g->AddBBAsSucc(C, B, true);
    g->AddBBAsSucc(I, B, false);
    g->AddBBAsSucc(D, C, true);
    g->AddBBAsSucc(I, C, false);
    g->AddBBAsSucc(E, D, true);
    g->AddBBAsSucc(G, E, true);
    g->AddBBAsSucc(F, E, false);
    g->AddBBAsSucc(C, F, false);
    g->AddBBAsSucc(H, G, true);
    g->AddBBAsSucc(J, I, true);
    g->AddBBAsSucc(K, J, true);
    g->AddBBAsSucc(L, K, true);
    g->AddBBAsSucc(O, L, true);
    g->AddBBAsSucc(M, L, false);
    g->AddBBAsSucc(N, M, true);
    g->AddBBAsSucc(J, N, true);
    g->AddBBAsSucc(P, O, true);
    g->AddBBAsSucc(I, P, true);
    g->AddBBAsSucc(G, O, false);

    g->SetFirstBB(A);

    auto l = LivenessAnalysis(g);

    auto order = l.CreateLinearOrder();

    ASSERT_EQ(order, std::vector<BB *>({A, B, C, D, E, F, I, J, K, L, M, N, O, P, G, H}));
}

TEST(LinOrder_Test, LinOrderTestSecond) {
    using namespace IRGen;

    InstructionBuilder builder;
    IRGenerator gen;

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *A = gen.CreateEmptyBB(g);
    BB *B = gen.CreateEmptyBB(g);
    BB *C = gen.CreateEmptyBB(g);
    BB *D = gen.CreateEmptyBB(g);
    BB *E = gen.CreateEmptyBB(g);
    BB *F = gen.CreateEmptyBB(g);
    BB *G = gen.CreateEmptyBB(g);
    BB *H = gen.CreateEmptyBB(g);

    g->AddBBAsSucc(B, A, true);
    g->AddBBAsSucc(C, B, false);
    g->AddBBAsSucc(D, B, true);
    g->AddBBAsSucc(F, C, true);
    g->AddBBAsSucc(E, C, false);
    g->AddBBAsSucc(F, D, true);
    g->AddBBAsSucc(G, F, true);
    g->AddBBAsSucc(H, G, true);
    g->AddBBAsSucc(B, G, false);
    g->AddBBAsSucc(A, H, true);

    g->SetFirstBB(A);

    auto l = LivenessAnalysis(g);

    auto order = l.CreateLinearOrder();

    ASSERT_EQ(order, std::vector<BB *>({A, B, D, C, F, G, H, E}));
}

// tests for correct liveness calculation, but not correct graph
TEST(Liveness_Analysis_Test, LivenessTestLecture_1) {
    using namespace IRGen;

    InstructionBuilder builder;
    IRGenerator gen;

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *b0 = gen.CreateEmptyBB(g);
    BB *b1 = gen.CreateEmptyBB(g);
    BB *b2 = gen.CreateEmptyBB(g);
    BB *b3 = gen.CreateEmptyBB(g);

    g->AddBBAsSucc(b1, b0, true);
    g->AddBBAsSucc(b2, b1, true);
    g->AddBBAsSucc(b1, b2, true);
    g->AddBBAsSucc(b3, b1, false);

    auto *val0 = builder.BuildMovI(uint64_t(1));
    auto *val1 = builder.BuildMovI(uint64_t(10));
    auto *val2 = builder.BuildMovI(uint64_t(20));
    b0->AddInstrBackward(val0);
    b0->AddInstrBackward(val1);
    b0->AddInstrBackward(val2);

    auto *val3 = builder.BuildPhi(InstrType::U64);
    auto *val4 = builder.BuildPhi(InstrType::U64);
    auto *val5 = builder.BuildCmp(InstrType::U64, val4, val0);
    auto *val6 = builder.BuildJa(b2, val5, b3);
    b1->AddInstrBackward(val3);
    b1->AddInstrBackward(val4);
    b1->AddInstrBackward(val5);
    b1->AddInstrBackward(val6);

    auto *val7 = builder.BuildMul(InstrType::U64, val3, val4);
    auto *val8 = builder.BuildMul(InstrType::U64, val4, val0);
    b2->AddInstrBackward(val7);
    b2->AddInstrBackward(val8);

    auto val9 = builder.BuildMul(InstrType::U64, val2, val3);
    auto val10 = builder.BuildRet(InstrType::VOID, nullptr);
    b3->AddInstrBackward(val9);
    b3->AddInstrBackward(val10);

    val3->SetIncoming(val7);
    val3->SetIncoming(val0);
    val4->SetIncoming(val8);
    val4->SetIncoming(val1);

    g->SetFirstBB(b0);
    auto l = LivenessAnalysis(g);

    l.run();

    ASSERT_EQ(val0->GetLinearNumber(), 0);
    ASSERT_EQ(val1->GetLinearNumber(), 1);
    ASSERT_EQ(val2->GetLinearNumber(), 2);
    ASSERT_EQ(val3->GetLinearNumber(), 3);
    ASSERT_EQ(val4->GetLinearNumber(), 4);
    ASSERT_EQ(val5->GetLinearNumber(), 5);
    ASSERT_EQ(val6->GetLinearNumber(), 6);
    ASSERT_EQ(val7->GetLinearNumber(), 7);
    ASSERT_EQ(val8->GetLinearNumber(), 8);
    ASSERT_EQ(val9->GetLinearNumber(), 9);
    ASSERT_EQ(val10->GetLinearNumber(), 10);

    ASSERT_EQ(val0->GetLiveNumber(), 2);
    ASSERT_EQ(val1->GetLiveNumber(), 4);
    ASSERT_EQ(val2->GetLiveNumber(), 6);
    ASSERT_EQ(val3->GetLiveNumber(), 8);
    ASSERT_EQ(val4->GetLiveNumber(), 8);
    ASSERT_EQ(val5->GetLiveNumber(), 10);
    ASSERT_EQ(val6->GetLiveNumber(), 12);
    ASSERT_EQ(val7->GetLiveNumber(), 16);
    ASSERT_EQ(val8->GetLiveNumber(), 18);
    ASSERT_EQ(val9->GetLiveNumber(), 22);
    ASSERT_EQ(val10->GetLiveNumber(), 24);

    ASSERT_EQ(val0->GetLiveRange(), LiveRange(2, 20));
    ASSERT_EQ(val1->GetLiveRange(), LiveRange(4, 8));
    ASSERT_EQ(val2->GetLiveRange(), LiveRange(6, 22));
    ASSERT_EQ(val3->GetLiveRange(), LiveRange(8, 22));
    ASSERT_EQ(val4->GetLiveRange(), LiveRange(8, 18));
    ASSERT_EQ(val5->GetLiveRange(), LiveRange(10, 12));
    ASSERT_EQ(val6->GetLiveRange(), LiveRange(12, 14));
    ASSERT_EQ(val7->GetLiveRange(), LiveRange(16, 20));
    ASSERT_EQ(val8->GetLiveRange(), LiveRange(18, 20));
    ASSERT_EQ(val9->GetLiveRange(), LiveRange(22, 24));
    ASSERT_EQ(val10->GetLiveRange(), LiveRange(24, 26));

    ASSERT_EQ(b0->GetLiveRange(), LiveRange(0, 8));
    ASSERT_EQ(b1->GetLiveRange(), LiveRange(8, 14));
    ASSERT_EQ(b2->GetLiveRange(), LiveRange(14, 20));
    ASSERT_EQ(b3->GetLiveRange(), LiveRange(20, 26));
}

TEST(Liveness_Analysis_Test, LivenessTestLecture_2) {
    using namespace IRGen;

    InstructionBuilder builder{};
    IRGenerator gen{};

    std::vector<InstrType> paramTypes{InstrType::U64, InstrType::U64};
    std::vector<ParameterInstr *> params;
    std::transform(paramTypes.begin(), paramTypes.end(), std::back_inserter(params),
                   [&builder](InstrType t) { return builder.BuildParam(t); });

    Function *g = gen.CreateFunction("test", InstrType::VOID, params);

    BB *b0 = gen.CreateEmptyBB(g);
    BB *b1 = gen.CreateEmptyBB(g);
    BB *b2 = gen.CreateEmptyBB(g);
    BB *b3 = gen.CreateEmptyBB(g);

    g->AddBBAsSucc(b1, b0, true);
    g->AddBBAsSucc(b2, b0, false);
    g->AddBBAsSucc(b3, b1, true);
    g->AddBBAsSucc(b3, b2, true);

    auto *val2 = builder.BuildAdd(InstrType::U64, g->GetParam(0), g->GetParam(1));
    auto *val3 = builder.BuildAdd(InstrType::U64, g->GetParam(0), g->GetParam(1));
    auto *valCmp = builder.BuildCmp(InstrType::U64, g->GetParam(0), g->GetParam(1));
    b0->AddInstrBackward(val2);
    b0->AddInstrBackward(val3);
    b0->AddInstrBackward(valCmp);

    auto *val4 = builder.BuildAdd(InstrType::U64, g->GetParam(0), g->GetParam(1));
    auto *val5 = builder.BuildAdd(InstrType::U64, g->GetParam(0), g->GetParam(1));
    auto *valJump1 = builder.BuildJump(b3);
    b1->AddInstrBackward(val4);
    b1->AddInstrBackward(val5);
    b1->AddInstrBackward(valJump1);

    auto *val6 = builder.BuildAdd(InstrType::U64, g->GetParam(0), g->GetParam(1));
    auto *val7 = builder.BuildAdd(InstrType::U64, g->GetParam(0), g->GetParam(1));
    auto *valJump2 = builder.BuildJump(b3);
    b2->AddInstrBackward(val6);
    b2->AddInstrBackward(val7);
    b2->AddInstrBackward(valJump2);

    auto *val8 = builder.BuildPhi(InstrType::U64);
    auto *val9 = builder.BuildPhi(InstrType::U64);
    auto *val10 = builder.BuildAdd(InstrType::U64, val8, val2);
    auto *val11 = builder.BuildAdd(InstrType::U64, val9, val3);
    auto *val12 = builder.BuildAdd(InstrType::U64, val8, val10);
    auto *valRet = builder.BuildRet(InstrType::VOID, nullptr);
    b3->AddInstrBackward(val8);
    b3->AddInstrBackward(val9);
    b3->AddInstrBackward(val10);
    b3->AddInstrBackward(val11);
    b3->AddInstrBackward(val12);
    b3->AddInstrBackward(valRet);

    val8->SetIncoming(val4);
    val8->SetIncoming(val6);
    val9->SetIncoming(val5);
    val9->SetIncoming(val7);

    g->SetFirstBB(b0);
    auto l = LivenessAnalysis(g);

    l.run();

    ASSERT_EQ(val2->GetLinearNumber(), 0);
    ASSERT_EQ(val3->GetLinearNumber(), 1);
    ASSERT_EQ(valCmp->GetLinearNumber(), 2);
    ASSERT_EQ(val4->GetLinearNumber(), 3);
    ASSERT_EQ(val5->GetLinearNumber(), 4);
    ASSERT_EQ(valJump1->GetLinearNumber(), 5);
    ASSERT_EQ(val6->GetLinearNumber(), 6);
    ASSERT_EQ(val7->GetLinearNumber(), 7);
    ASSERT_EQ(valJump2->GetLinearNumber(), 8);
    ASSERT_EQ(val8->GetLinearNumber(), 9);
    ASSERT_EQ(val9->GetLinearNumber(), 10);
    ASSERT_EQ(val10->GetLinearNumber(), 11);
    ASSERT_EQ(val11->GetLinearNumber(), 12);
    ASSERT_EQ(val12->GetLinearNumber(), 13);
    ASSERT_EQ(valRet->GetLinearNumber(), 14);

    ASSERT_EQ(val2->GetLiveNumber(), 2);
    ASSERT_EQ(val3->GetLiveNumber(), 4);
    ASSERT_EQ(valCmp->GetLiveNumber(), 6);
    ASSERT_EQ(val4->GetLiveNumber(), 10);
    ASSERT_EQ(val5->GetLiveNumber(), 12);
    ASSERT_EQ(valJump1->GetLiveNumber(), 14);
    ASSERT_EQ(val6->GetLiveNumber(), 18);
    ASSERT_EQ(val7->GetLiveNumber(), 20);
    ASSERT_EQ(valJump2->GetLiveNumber(), 22);
    ASSERT_EQ(val8->GetLiveNumber(), 24);
    ASSERT_EQ(val9->GetLiveNumber(), 24);
    ASSERT_EQ(val10->GetLiveNumber(), 26);
    ASSERT_EQ(val11->GetLiveNumber(), 28);
    ASSERT_EQ(val12->GetLiveNumber(), 30);
    ASSERT_EQ(valRet->GetLiveNumber(), 32);

    ASSERT_EQ(val2->GetLiveRange(), LiveRange(2, 26));
    ASSERT_EQ(val3->GetLiveRange(), LiveRange(4, 28));
    ASSERT_EQ(val4->GetLiveRange(), LiveRange(10, 16));
    ASSERT_EQ(val5->GetLiveRange(), LiveRange(12, 16));
    ASSERT_EQ(val6->GetLiveRange(), LiveRange(18, 24));
    ASSERT_EQ(val7->GetLiveRange(), LiveRange(20, 24));
    ASSERT_EQ(val8->GetLiveRange(), LiveRange(24, 30));
    ASSERT_EQ(val9->GetLiveRange(), LiveRange(24, 28));
    ASSERT_EQ(val10->GetLiveRange(), LiveRange(26, 30));
    ASSERT_EQ(val11->GetLiveRange(), LiveRange(28, 30));
    ASSERT_EQ(val12->GetLiveRange(), LiveRange(30, 32));

    ASSERT_EQ(b0->GetLiveRange(), LiveRange(0, 8));
    ASSERT_EQ(b1->GetLiveRange(), LiveRange(8, 16));
    ASSERT_EQ(b2->GetLiveRange(), LiveRange(16, 24));
    ASSERT_EQ(b3->GetLiveRange(), LiveRange(24, 34));
}

TEST(Liveness_Analysis_Test, LivenessTest3) {
    using namespace IRGen;

    InstructionBuilder builder;
    IRGenerator gen;

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *b0 = gen.CreateEmptyBB(g);
    BB *b1 = gen.CreateEmptyBB(g);
    BB *b2 = gen.CreateEmptyBB(g);
    BB *b3 = gen.CreateEmptyBB(g);

    g->AddBBAsSucc(b1, b0, true);
    g->AddBBAsSucc(b2, b1, true);
    g->AddBBAsSucc(b3, b1, false);
    g->AddBBAsSucc(b1, b2, true);
    g->AddBBAsSucc(b3, b2, false);

    auto *val0 = builder.BuildMovI(uint64_t(1));
    auto *val1 = builder.BuildMovI(uint64_t(10));
    auto *valJump = builder.BuildJump(b1);
    auto *val2 = builder.BuildPhi(InstrType::U64);
    auto *val3 = builder.BuildAddI(val2, uint64_t(1));
    auto *val4 = builder.BuildCmp(InstrType::U64, val3, val1);
    auto *valJA1 = builder.BuildJa(b2, val4, b3);
    auto *val5 = builder.BuildAddI(val3, uint64_t(2));
    auto *val6 = builder.BuildCmp(InstrType::U64, val5, val1);
    auto *valJA2 = builder.BuildJa(b1, val4, b3);
    auto *valRet = builder.BuildRet(InstrType::VOID, nullptr);

    b0->AddInstrBackward(val0);
    b0->AddInstrBackward(val1);
    b0->AddInstrBackward(valJump);

    b1->AddInstrBackward(val2);
    b1->AddInstrBackward(val3);
    b1->AddInstrBackward(val4);
    b1->AddInstrBackward(valJA1);

    b2->AddInstrBackward(val5);
    b2->AddInstrBackward(val6);
    b2->AddInstrBackward(valJA2);

    b3->AddInstrBackward(valRet);

    val2->SetIncoming(val0);
    val2->SetIncoming(val5);

    g->SetFirstBB(b0);

    auto l = LivenessAnalysis(g);
    l.run();

    ASSERT_EQ(val0->GetLiveRange(), LiveRange(2, 8));
    ASSERT_EQ(val1->GetLiveRange(), LiveRange(4, 24));
    ASSERT_EQ(valJump->GetLiveRange(), LiveRange(6, 8));
    ASSERT_EQ(val2->GetLiveRange(), LiveRange(8, 10));
    ASSERT_EQ(val3->GetLiveRange(), LiveRange(10, 18));
    ASSERT_EQ(val4->GetLiveRange(), LiveRange(12, 22));
    ASSERT_EQ(valJA1->GetLiveRange(), LiveRange(14, 16));
    ASSERT_EQ(val5->GetLiveRange(), LiveRange(18, 24));
    ASSERT_EQ(val6->GetLiveRange(), LiveRange(20, 22));
    ASSERT_EQ(valJA2->GetLiveRange(), LiveRange(22, 24));
    ASSERT_EQ(valRet->GetLiveRange(), LiveRange(26, 28));

    ASSERT_EQ(b0->GetLiveRange(), LiveRange(0, 8));
    ASSERT_EQ(b1->GetLiveRange(), LiveRange(8, 16));
    ASSERT_EQ(b2->GetLiveRange(), LiveRange(16, 24));
    ASSERT_EQ(b3->GetLiveRange(), LiveRange(24, 28));
}