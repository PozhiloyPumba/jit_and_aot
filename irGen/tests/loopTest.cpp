#include "IRBuilder.hpp"
#include "function.hpp"
#include <gtest/gtest.h>
#include <loopAnalyzer.hpp>

TEST(Loop_Test, SimpleFirstExample) {
    using namespace IRGen;

    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *A = gen.CreateEmptyBB(g);
    BB *B = gen.CreateEmptyBB(g);
    BB *C = gen.CreateEmptyBB(g);
    BB *D = gen.CreateEmptyBB(g);
    BB *E = gen.CreateEmptyBB(g);
    g->AddBBAsSucc(B, A, true);
    g->AddBBAsSucc(C, B, false);
    g->AddBBAsSucc(D, B, true);
    g->AddBBAsSucc(E, D, true);
    g->AddBBAsSucc(B, E, true);
    g->SetFirstBB(A);
    LoopAnalyzer l(g);
    l.BuildLoopTree();

    const auto &loops = l.GetLoops();
    ASSERT_EQ(loops.size(), 1);

    auto loopB = loops.at(B);
    std::unordered_set<Loop *> empty;
    ASSERT_EQ(loopB->GetHeader(), B);
    ASSERT_EQ(loopB->GetLatches(), std::unordered_set({E}));
    ASSERT_EQ(loopB->GetOuterLoop(), l.GetRootLoop());
    ASSERT_EQ(loopB->GetInnerLoops(), empty);
    ASSERT_EQ(loopB->GetBody(), std::unordered_set({B, D, E}));
    EXPECT_TRUE(loopB->GetReducible());
}

TEST(Loop_Test, SimpleSecondExample) {
    using namespace IRGen;

    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *A = gen.CreateEmptyBB(g);
    BB *B = gen.CreateEmptyBB(g);
    BB *C = gen.CreateEmptyBB(g);
    BB *D = gen.CreateEmptyBB(g);
    BB *E = gen.CreateEmptyBB(g);
    BB *F = gen.CreateEmptyBB(g);
    g->AddBBAsSucc(B, A, true);
    g->AddBBAsSucc(C, B, true);
    g->AddBBAsSucc(D, C, true);
    g->AddBBAsSucc(F, C, false);
    g->AddBBAsSucc(E, D, true);
    g->AddBBAsSucc(F, D, false);
    g->AddBBAsSucc(B, E, true);
    g->SetFirstBB(A);
    LoopAnalyzer l(g);
    l.BuildLoopTree();

    auto loops = l.GetLoops();
    ASSERT_EQ(loops.size(), 1);

    auto loopB = loops.at(B);
    std::unordered_set<Loop *> empty;
    ASSERT_EQ(loopB->GetHeader(), B);
    ASSERT_EQ(loopB->GetLatches(), std::unordered_set({E}));
    ASSERT_EQ(loopB->GetOuterLoop(), l.GetRootLoop());
    ASSERT_EQ(loopB->GetInnerLoops(), empty);
    ASSERT_EQ(loopB->GetBody(), std::unordered_set({B, C, D, E}));
    EXPECT_TRUE(loopB->GetReducible());
}

TEST(Loop_Test, ThirdSecondExample) {
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
    g->AddBBAsSucc(B, A, true);
    g->AddBBAsSucc(C, B, true);
    g->AddBBAsSucc(D, B, false);
    g->AddBBAsSucc(F, C, false);
    g->AddBBAsSucc(E, C, true);
    g->AddBBAsSucc(E, D, true);
    g->AddBBAsSucc(G, E, true);
    g->AddBBAsSucc(H, G, true);
    g->AddBBAsSucc(B, G, false);
    g->AddBBAsSucc(A, H, true);

    g->SetFirstBB(A);
    LoopAnalyzer l(g);
    l.BuildLoopTree();

    ASSERT_TRUE(A->IsLoopHeader());
    ASSERT_TRUE(B->IsLoopHeader());
    ASSERT_FALSE(C->IsLoopHeader());
    ASSERT_FALSE(D->IsLoopHeader());
    ASSERT_FALSE(E->IsLoopHeader());
    ASSERT_FALSE(F->IsLoopHeader());
    ASSERT_FALSE(G->IsLoopHeader());
    ASSERT_FALSE(H->IsLoopHeader());

    auto loops = l.GetLoops();
    ASSERT_EQ(loops.size(), 2);

    auto loopA = loops.at(A);
    auto loopB = loops.at(B);
    std::unordered_set<Loop *> empty;
    ASSERT_EQ(loopA->GetHeader(), A);
    ASSERT_EQ(loopA->GetLatches(), std::unordered_set({H}));
    ASSERT_EQ(loopA->GetInnerLoops(), std::unordered_set({loopB}));
    ASSERT_EQ(loopA->GetOuterLoop(), l.GetRootLoop());
    ASSERT_EQ(loopA->GetBody(), std::unordered_set({A, H}));
    EXPECT_TRUE(loopA->GetReducible());

    ASSERT_EQ(loopB->GetHeader(), B);
    ASSERT_EQ(loopB->GetLatches(), std::unordered_set({G}));
    ASSERT_EQ(loopB->GetOuterLoop(), loopA);
    ASSERT_EQ(loopB->GetInnerLoops(), empty);
    ASSERT_EQ(loopB->GetBody(), std::unordered_set({B, C, D, E, G}));
    EXPECT_TRUE(loopB->GetReducible());
}

TEST(Loop_Test, FirstExample) {
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

    LoopAnalyzer l(g);
    l.BuildLoopTree();

    ASSERT_FALSE(A->IsLoopHeader());
    ASSERT_FALSE(B->IsLoopHeader());
    ASSERT_FALSE(C->IsLoopHeader());
    ASSERT_FALSE(D->IsLoopHeader());
    ASSERT_FALSE(E->IsLoopHeader());
    ASSERT_FALSE(F->IsLoopHeader());
    ASSERT_FALSE(G->IsLoopHeader());

    auto loops = l.GetLoops();
    ASSERT_EQ(loops.size(), 0);
}

TEST(Loop_Test, SecondExample) {
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
    g->AddBBAsSucc(C, J, true);
    g->AddBBAsSucc(C, B, false);
    g->AddBBAsSucc(D, C, true);
    g->AddBBAsSucc(E, D, true);
    g->AddBBAsSucc(C, D, false);
    g->AddBBAsSucc(F, E, true);
    g->AddBBAsSucc(G, F, true);
    g->AddBBAsSucc(E, F, false);
    g->AddBBAsSucc(I, G, true);
    g->AddBBAsSucc(H, G, false);
    g->AddBBAsSucc(B, H, true);
    g->AddBBAsSucc(K, I, true);

    g->SetFirstBB(A);

    LoopAnalyzer l(g);
    l.BuildLoopTree();
    auto loops = l.GetLoops();
    ASSERT_EQ(loops.size(), 3);

    auto loopB = loops.at(B);
    auto loopC = loops.at(C);
    auto loopE = loops.at(E);
    ASSERT_FALSE(A->IsLoopHeader());
    ASSERT_TRUE(B->IsLoopHeader());
    ASSERT_TRUE(C->IsLoopHeader());
    ASSERT_FALSE(D->IsLoopHeader());
    ASSERT_TRUE(E->IsLoopHeader());
    ASSERT_FALSE(F->IsLoopHeader());
    ASSERT_FALSE(G->IsLoopHeader());
    ASSERT_FALSE(H->IsLoopHeader());
    ASSERT_FALSE(I->IsLoopHeader());
    ASSERT_FALSE(J->IsLoopHeader());
    ASSERT_FALSE(K->IsLoopHeader());

    std::unordered_set<Loop *> empty;
    ASSERT_EQ(loopB->GetHeader(), B);
    ASSERT_EQ(loopB->GetLatches(), std::unordered_set({H}));
    ASSERT_EQ(loopB->GetInnerLoops(), std::unordered_set({loopC, loopE}));
    ASSERT_EQ(loopB->GetBody(), std::unordered_set({B, J, G, H}));
    ASSERT_EQ(loopB->GetOuterLoop(), l.GetRootLoop());
    EXPECT_TRUE(loopB->GetReducible());

    ASSERT_EQ(loopC->GetHeader(), C);
    ASSERT_EQ(loopC->GetLatches(), std::unordered_set({D}));
    ASSERT_EQ(loopC->GetInnerLoops(), empty);
    ASSERT_EQ(loopC->GetOuterLoop(), loopB);
    ASSERT_EQ(loopC->GetBody(), std::unordered_set({C, D}));
    EXPECT_TRUE(loopC->GetReducible());

    ASSERT_EQ(loopE->GetHeader(), E);
    ASSERT_EQ(loopE->GetLatches(), std::unordered_set({F}));
    ASSERT_EQ(loopE->GetInnerLoops(), empty);
    ASSERT_EQ(loopE->GetOuterLoop(), loopB);
    ASSERT_EQ(loopE->GetBody(), std::unordered_set({E, F}));
    EXPECT_TRUE(loopE->GetReducible());
}

TEST(Loop_Test, ThirdExample) {
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
    g->AddBBAsSucc(C, B, false);
    g->AddBBAsSucc(E, B, true);
    g->AddBBAsSucc(D, C, true);
    g->AddBBAsSucc(G, D, true);
    g->AddBBAsSucc(F, E, false);
    g->AddBBAsSucc(D, E, true);
    g->AddBBAsSucc(H, F, true);
    g->AddBBAsSucc(B, F, false);
    g->AddBBAsSucc(I, G, true);
    g->AddBBAsSucc(C, G, false);
    g->AddBBAsSucc(G, H, true);
    g->AddBBAsSucc(I, H, false);

    g->SetFirstBB(A);

    LoopAnalyzer l(g);
    l.BuildLoopTree();
    auto loops = l.GetLoops();
    ASSERT_EQ(loops.size(), 2);

    auto loopB = loops.at(B);

    std::unordered_set<Loop *> empty;
    ASSERT_EQ(loopB->GetHeader(), B);
    ASSERT_EQ(loopB->GetLatches(), std::unordered_set({F}));
    ASSERT_EQ(loopB->GetInnerLoops(), empty);
    ASSERT_EQ(loopB->GetOuterLoop(), l.GetRootLoop());
    ASSERT_EQ(loopB->GetBody(), std::unordered_set({B, E, F}));
    EXPECT_TRUE(loopB->GetReducible());

    Loop *IrrLoop = nullptr;
    for (auto &p : loops)
        if (p.first != B)
            IrrLoop = p.second;

    ASSERT_EQ(IrrLoop->GetInnerLoops(), empty);
    EXPECT_FALSE(IrrLoop->GetReducible());
}
