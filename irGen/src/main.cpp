#include "IRBuilder.hpp"
#include "function.hpp"
#include "loopAnalyzer.hpp"
#include <algorithm>
#include <iostream>
#include <memory>

int main() {
    using namespace IRGen;
    try {
        InstructionBuilder builder;
        IRGenerator gen;

        std::vector<InstrType> paramTypes{InstrType::U32};
        std::vector<ParameterInstr *> params;
        std::transform(paramTypes.begin(), paramTypes.end(), std::back_inserter(params),
                       [&builder](InstrType t) { return builder.BuildParam(t); });

        Function *g = gen.CreateFunction("fact", InstrType::U64, params);
        BB *b1 = gen.CreateEmptyBB(g);
        auto *val1 = builder.BuildMovI(uint64_t(1));
        auto *val2 = builder.BuildMovI(uint64_t(2));
        auto *val3 = builder.BuildCast(InstrType::U64, g->GetParam(0));
        b1->AddInstrBackward(val1);
        b1->AddInstrBackward(val2);
        b1->AddInstrBackward(val3);

        BB *b2 = gen.CreateEmptyBB(g);
        BB *b3 = gen.CreateEmptyBB(g);
        BB *b4 = gen.CreateEmptyBB(g);
        b1->AddSucc(b2, true);
        b2->AddPred(b1);
        b3->AddPred(b2);
        b3->AddSucc(b2, true);
        b2->AddPred(b3);
        b2->AddSucc(b3, true);
        b2->AddSucc(b4, false);
        b4->AddPred(b2);
        auto *phi1 = builder.BuildPhi(InstrType::U64);
        auto *phi2 = builder.BuildPhi(InstrType::U64);
        b2->AddInstrBackward(phi1);
        b2->AddInstrBackward(phi2);
        auto *valCmp = builder.BuildCmp(InstrType::U64, phi1, val2);
        b2->AddInstrBackward(valCmp);
        b2->AddInstrBackward(builder.BuildJa(b4, valCmp, b3));
        auto *valMul = builder.BuildMul(InstrType::U64, val1, val1);
        b3->AddInstrBackward(valMul);
        auto *valAdd = builder.BuildAddI(valMul, uint64_t(1));
        b3->AddInstrBackward(valAdd);
        b3->AddInstrBackward(builder.BuildJump(b2));
        b4->AddInstrBackward(builder.BuildRet(InstrType::U64, phi2));
        phi1->SetIncoming(val2);
        phi1->SetIncoming(valAdd);
        phi2->SetIncoming(valMul);
        phi2->SetIncoming(val1);
        g->dump();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}