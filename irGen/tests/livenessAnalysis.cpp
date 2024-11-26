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