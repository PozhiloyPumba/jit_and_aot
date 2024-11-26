#include "livenessAnalysis.hpp"
#include "IRBuilder.hpp"
#include "function.hpp"
#include <gtest/gtest.h>

TEST(Liveness_Analysis_Test, LinOrderTestFirst) {
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

TEST(Liveness_Analysis_Test, LinOrderTestSecond) {
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

TEST(Liveness_Analysis_Test, LivenessTestFirst) {
    using namespace IRGen;

    InstructionBuilder builder;
    IRGenerator gen;

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *b1 = gen.CreateEmptyBB(g);
    BB *b2 = gen.CreateEmptyBB(g);
    BB *b3 = gen.CreateEmptyBB(g);
    BB *b4 = gen.CreateEmptyBB(g);

    g->AddBBAsSucc(b2, b1, true);
    g->AddBBAsSucc(b2, b3, true);
    g->AddBBAsSucc(b3, b2, true);
    g->AddBBAsSucc(b4, b2, false);

    auto *val1 = builder.BuildMovI(uint64_t(1));
    auto *val2 = builder.BuildMovI(uint64_t(2));
    auto *val3 = builder.BuildMovI(uint64_t(10));
    b1->AddInstrBackward(val1);
    b1->AddInstrBackward(val2);
    b1->AddInstrBackward(val3);

    auto *phi1 = builder.BuildPhi(InstrType::U64);
    auto *phi2 = builder.BuildPhi(InstrType::U64);
    auto *valCmp = builder.BuildCmp(InstrType::U64, phi1, val2);
    auto *jumpInstr = builder.BuildJa(b4, valCmp, b3);
    b2->AddInstrBackward(phi1);
    b2->AddInstrBackward(phi2);
    b2->AddInstrBackward(valCmp);
    b2->AddInstrBackward(jumpInstr);

    auto *valMul = builder.BuildMul(InstrType::U64, val1, val1);
    auto *valAdd = builder.BuildAddI(valMul, uint64_t(1));
    auto *valJump = builder.BuildJump(b2);
    b3->AddInstrBackward(valMul);
    b3->AddInstrBackward(valAdd);
    b3->AddInstrBackward(valJump);

    auto retInstr = builder.BuildRet(InstrType::U64, phi2);
    b4->AddInstrBackward(retInstr);

    phi1->SetIncoming(val2);
    phi1->SetIncoming(valAdd);
    phi2->SetIncoming(valMul);
    phi2->SetIncoming(val1);

    g->SetFirstBB(b1);
    auto l = LivenessAnalysis(g);

    l.run();

    ASSERT_EQ(val1->GetLinearNumber(), 0);
    ASSERT_EQ(val1->GetLiveNumber(), 2);
    ASSERT_EQ(val2->GetLinearNumber(), 1);
    ASSERT_EQ(val2->GetLiveNumber(), 4);
    ASSERT_EQ(val3->GetLinearNumber(), 2);
    ASSERT_EQ(val3->GetLiveNumber(), 6);

    ASSERT_EQ(phi1->GetLinearNumber(), 3);
    ASSERT_EQ(phi1->GetLiveNumber(), 8);
    ASSERT_EQ(phi2->GetLinearNumber(), 4);
    ASSERT_EQ(phi2->GetLiveNumber(), 8);
    ASSERT_EQ(valCmp->GetLinearNumber(), 5);
    ASSERT_EQ(valCmp->GetLiveNumber(), 10);
    ASSERT_EQ(jumpInstr->GetLinearNumber(), 6);
    ASSERT_EQ(jumpInstr->GetLiveNumber(), 12);

    ASSERT_EQ(valMul->GetLinearNumber(), 7);
    ASSERT_EQ(valMul->GetLiveNumber(), 16);
    ASSERT_EQ(valAdd->GetLinearNumber(), 8);
    ASSERT_EQ(valAdd->GetLiveNumber(), 18);
    ASSERT_EQ(valJump->GetLinearNumber(), 9);
    ASSERT_EQ(valJump->GetLiveNumber(), 20);

    ASSERT_EQ(retInstr->GetLinearNumber(), 10);
    ASSERT_EQ(retInstr->GetLiveNumber(), 24);

    [[maybe_unused]] auto print = [](auto *instr) {
        std::cout << instr->GetLiveRange().start << " " << instr->GetLiveRange().end << std::endl;
    };

    ASSERT_EQ(val1->GetLiveRange(), LiveRange(0, 22));
}