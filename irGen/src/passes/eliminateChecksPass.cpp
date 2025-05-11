#include "passes/eliminateChecksPass.hpp"
#include "IRBuilder.hpp"
#include <stdexcept>
#include <iostream>
#include <unordered_set>
#include <ranges>
#include <utility>
#include <functional>


namespace IRGen {

void EliminateChecksPass::Run(Function *func) const {
    auto &builder = InstructionBuilder::GetInstance();
	func->buildDominatorTree();

	std::unordered_set<Instruction*> delayDelete;
	std::vector<Opcode> checked_opcodes = {Opcode::NULLCHECK, Opcode::BOUNDSCHECK};

    enum Color { WHITE = 0, BLACK = 1, GREY = 1 << 1 };
    std::unordered_map<BB *, unsigned> markers_;

    std::vector<BB *> po;
    std::function<void(BB *)> poRun;
    poRun = [&poRun, &markers_, &po](BB *bb) {
        markers_[bb] = BLACK;
        for (auto *s : bb->GetSuccs()) {
            if (s && !markers_[s]) {
                poRun(s);
            }
        }
        po.push_back(bb);
    };
    poRun(func->GetFirstBB());

	using namespace std::ranges;

	for (auto *bb : po | views::reverse) {
		for (auto curIt = bb->GetBeginBB(), endIt = bb->GetEndBB()->GetNextInstr(); curIt != endIt;
			curIt = curIt->GetNextInstr()) {
			if (std::find(checked_opcodes.begin(), checked_opcodes.end(), curIt->GetOpcode()) == checked_opcodes.end()) {
				continue;
			}

			auto *input = curIt->GetInputs()[0];
			for (auto& user: input->GetUsers() | std::views::filter(
					[&checked_opcodes, curIt](auto *instr){
						return (curIt->GetOpcode() == instr->GetOpcode()) &&
							instr->DominatedBy(curIt) &&
							(std::find(checked_opcodes.begin(), checked_opcodes.end(), instr->GetOpcode()) != checked_opcodes.end());
					})
			) {
				if (curIt->GetOpcode() == Opcode::BOUNDSCHECK) {
					if (std::get<int64_t>(static_cast<ValAndImmInstr *>(user)->GetImm()) < 
						std::get<int64_t>(static_cast<ValAndImmInstr *>(curIt)->GetImm())) {
						continue;
					}
				}
				delayDelete.insert(user);
			}
		}
	}
	for (auto *instr: delayDelete) {
		auto *bb = instr->GetBB();
		bb->AssasinateInstr(instr);
	}
}

}