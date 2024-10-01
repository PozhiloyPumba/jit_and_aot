#include <iostream>
#include <memory>
#include "IRBuilder.hpp"
#include "graph.hpp"

int main() {
	using namespace IRGen;
	try {
		InstructionBuilder builder;
		IRGenerator gen;
		gen.CreateGraph();
		Graph *g = gen.GetGraph();
		g->SetName("fact");
		g->SetRetType(InstrType::U64);
		g->SetParams({std::make_pair(InstrType::U32, 0)});
		BB *b1 = gen.CreateEmptyBB();
		b1->AddInstrBackward(builder.BuildMovI(InstrType::U64, 0, uint64_t(1)));
		b1->AddInstrBackward(builder.BuildMovI(InstrType::U64, 1, uint64_t(2)));
		b1->AddInstrBackward(builder.BuildCast(InstrType::U64, 2, InstrType::U32, 0));

		BB *b2 = gen.CreateEmptyBB();
		BB *b3 = gen.CreateEmptyBB();
		BB *b4 = gen.CreateEmptyBB();
		b1->AddSucc(b2);
		b2->AddPred(b1);
		b3->AddPred(b2);
		b3->AddSucc(b2);
		b2->AddPred(b3);
		b2->AddSucc(b3);
		b2->AddSucc(b4);
		b4->AddPred(b2);
		b2->AddInstrBackward(builder.BuildCmp(InstrType::U64, 1, 2));
		b2->AddInstrBackward(builder.BuildJa(b4));
		b3->AddInstrBackward(builder.BuildMul(InstrType::U64, 0, 0, 1));
		b3->AddInstrBackward(builder.BuildAddI(InstrType::U64, 1, 1, uint64_t(1)));
		b3->AddInstrBackward(builder.BuildJump(b2));
		b4->AddInstrBackward(builder.BuildRet(InstrType::U64, 0));

		g->dump();
	} catch(const std::exception& e) {
    	std::cerr<<"Exception: "<<e.what()<<std::endl;
	}
}