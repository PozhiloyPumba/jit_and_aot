#include "IRBuilder.hpp"
#include "function.hpp"
#include "passes/peepholePass.hpp"
#include <gtest/gtest.h>

TEST(PeepholePassTest, sub_test) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *b0 = gen.CreateEmptyBB(g);

    g->SetFirstBB(b0);

    auto *val0 = builder.BuildMovI(uint64_t(1));
    auto *val1 = builder.BuildSub(InstrType::U64, val0, val0);
    auto *val2 = builder.BuildAdd(InstrType::U64, val0, val1);
    auto *val3 = builder.BuildSubI(val0, uint64_t(0));
    auto *val4 = builder.BuildAdd(InstrType::U64, val3, val3);
    auto *val5 = builder.BuildSubI(val3, uint64_t(1));
    b0->AddInstrBackward(val0);
    b0->AddInstrBackward(val1);
    b0->AddInstrBackward(val2);
    b0->AddInstrBackward(val3);
    b0->AddInstrBackward(val4);
    b0->AddInstrBackward(val5);

    PeepHolePass p;
    p.Run(g);

    std::vector<Opcode> types{Opcode::MOVI, Opcode::MOVI, Opcode::ADD, Opcode::ADD, Opcode::DEC};

    Instruction *cur = b0->GetBeginBB();
    for (auto &t : types) {
        ASSERT_EQ(cur->GetOpcode(), t);
        cur = cur->GetNextInstr();
    }

    ASSERT_EQ(cur, nullptr);

    // g->dump();
}

TEST(PeepholePassTest, and_test) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *b0 = gen.CreateEmptyBB(g);

    g->SetFirstBB(b0);

    auto *val0 = builder.BuildMovI(uint64_t(1));
    auto *val1 = builder.BuildAnd(InstrType::U64, val0, val0);
    auto *val2 = builder.BuildAdd(InstrType::U64, val0, val1);
    auto *val3 = builder.BuildAndI(val0, uint64_t(0));
    auto *val4 = builder.BuildAndI(val3, uint64_t(0xFFFFFFFFFFFFFFFF));
    auto *val5 = builder.BuildSubI(val4, uint64_t(1));
    b0->AddInstrBackward(val0);
    b0->AddInstrBackward(val1);
    b0->AddInstrBackward(val2);
    b0->AddInstrBackward(val3);
    b0->AddInstrBackward(val4);
    b0->AddInstrBackward(val5);

    PeepHolePass p;
    p.Run(g);

    std::vector<Opcode> types{Opcode::MOVI, Opcode::ADD, Opcode::MOVI, Opcode::DEC};

    Instruction *cur = b0->GetBeginBB();
    for (auto &t : types) {
        ASSERT_EQ(cur->GetOpcode(), t);
        cur = cur->GetNextInstr();
    }

    ASSERT_EQ(cur, nullptr);

    // g->dump();
}

TEST(PeepholePassTest, shr_test) {
    using namespace IRGen;

    auto &builder = InstructionBuilder::GetInstance();
    auto &gen = IRGenerator::GetInstance();

    Function *g = gen.CreateFunction("test", InstrType::VOID, {});

    BB *b0 = gen.CreateEmptyBB(g);

    g->SetFirstBB(b0);

    auto *val0 = builder.BuildMovI(uint32_t(1));
    auto *val1 = builder.BuildShrI(val0, uint32_t(0));
    auto *val2 = builder.BuildShrI(val1, uint32_t(2));
    auto *val3 = builder.BuildShrI(val2, uint32_t(33));
    auto *val4 = builder.BuildShrI(val0, uint32_t(3));
    auto *val5 = builder.BuildShlI(val4, uint32_t(3));
    b0->AddInstrBackward(val0);
    b0->AddInstrBackward(val1);
    b0->AddInstrBackward(val2);
    b0->AddInstrBackward(val3);
    b0->AddInstrBackward(val4);
    b0->AddInstrBackward(val5);

    PeepHolePass p;
    p.Run(g);

    std::vector<Opcode> types{Opcode::MOVI, Opcode::SHRI, Opcode::MOVI, Opcode::ANDI};

    Instruction *cur = b0->GetBeginBB();
    for (auto &t : types) {
        ASSERT_EQ(cur->GetOpcode(), t);
        cur = cur->GetNextInstr();
    }

    ASSERT_EQ(cur, nullptr);

    // g->dump();
}