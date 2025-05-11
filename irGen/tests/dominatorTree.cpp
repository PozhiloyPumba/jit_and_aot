#include "IRBuilder.hpp"
#include "function.hpp"
#include <gtest/gtest.h>
#include "helpers/defines.hpp"


#undef GTEST_GROUP_CUSTOM
#define GTEST_GROUP_CUSTOM DominatorTree_Test


DEFINE_GTEST_CUSTOM(FirstExample) {
    using namespace IRGen;

    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *A = gen.CreateEmptyBB(g);
    BB *B = gen.CreateEmptyBB(g);
    BB *C = gen.CreateEmptyBB(g);
    BB *D = gen.CreateEmptyBB(g);
    BB *E = gen.CreateEmptyBB(g);
    BB *F = gen.CreateEmptyBB(g);
    BB *G = gen.CreateEmptyBB(g);
    g->AddBBAsSucc(B, A, true);
    g->AddBBAsSucc(C, B, false);
    g->AddBBAsSucc(F, B, true);
    g->AddBBAsSucc(D, C, true);
    g->AddBBAsSucc(E, F, false);
    g->AddBBAsSucc(G, F, true);
    g->AddBBAsSucc(D, G, true);
    g->SetFirstBB(A);
    auto d = g->buildDominatorTree();

    std::unordered_set<BB *> empty;
    ASSERT_EQ(d[A], std::unordered_set({B}));
    ASSERT_EQ(d[B], std::unordered_set({C, D, F}));
    ASSERT_EQ(d[C], empty);
    ASSERT_EQ(d[D], empty);
    ASSERT_EQ(d[E], empty);
    ASSERT_EQ(d[F], std::unordered_set({E, G}));
    ASSERT_EQ(d[G], empty);
}

DEFINE_GTEST_CUSTOM(SecondExample) {
    using namespace IRGen;

    auto &gen = IRGenerator::GetInstance();

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

    g->AddBBAsSucc(B, A, true);
    g->AddBBAsSucc(J, B, true);
    g->AddBBAsSucc(C, B, false);
    g->AddBBAsSucc(D, C, true);
    g->AddBBAsSucc(E, D, true);
    g->AddBBAsSucc(C, D, false);
    g->AddBBAsSucc(F, E, true);
    g->AddBBAsSucc(G, F, true);
    g->AddBBAsSucc(E, F, false);
    g->AddBBAsSucc(I, G, true);
    g->AddBBAsSucc(H, G, false);
    g->AddBBAsSucc(A, H, true);
    g->AddBBAsSucc(A, H, true);
    g->AddBBAsSucc(K, I, true);
    g->AddBBAsSucc(C, J, true);

    g->SetFirstBB(A);
    auto d = g->buildDominatorTree();

    std::unordered_set<BB *> empty;
    ASSERT_EQ(d[A], std::unordered_set({B}));
    ASSERT_EQ(d[B], std::unordered_set({C, J}));
    ASSERT_EQ(d[C], std::unordered_set({D}));
    ASSERT_EQ(d[D], std::unordered_set({E}));
    ASSERT_EQ(d[E], std::unordered_set({F}));
    ASSERT_EQ(d[F], std::unordered_set({G}));
    ASSERT_EQ(d[G], std::unordered_set({H, I}));
    ASSERT_EQ(d[H], empty);
    ASSERT_EQ(d[I], std::unordered_set({K}));
    ASSERT_EQ(d[J], empty);
    ASSERT_EQ(d[K], empty);
}

DEFINE_GTEST_CUSTOM(ThirdExample) {
    using namespace IRGen;

    auto &gen = IRGenerator::GetInstance();

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

    g->AddBBAsSucc(B, A, true);
    g->AddBBAsSucc(C, B, true);
    g->AddBBAsSucc(E, B, false);
    g->AddBBAsSucc(D, C, true);
    g->AddBBAsSucc(G, D, true);
    g->AddBBAsSucc(F, E, false);
    g->AddBBAsSucc(D, E, true);
    g->AddBBAsSucc(D, E, true);
    g->AddBBAsSucc(H, F, true);
    g->AddBBAsSucc(B, F, false);
    g->AddBBAsSucc(I, G, true);
    g->AddBBAsSucc(C, G, false);
    g->AddBBAsSucc(G, H, true);
    g->AddBBAsSucc(I, H, false);

    g->SetFirstBB(A);
    auto d = g->buildDominatorTree();

    std::unordered_set<BB *> empty;

    ASSERT_EQ(d[A], std::unordered_set({B}));
    ASSERT_EQ(d[B], std::unordered_set({C, D, E, G, I}));
    ASSERT_EQ(d[C], empty);
    ASSERT_EQ(d[D], empty);
    ASSERT_EQ(d[E], std::unordered_set({F}));
    ASSERT_EQ(d[F], std::unordered_set({H}));
    ASSERT_EQ(d[G], empty);
    ASSERT_EQ(d[H], empty);
    ASSERT_EQ(d[I], empty);
}