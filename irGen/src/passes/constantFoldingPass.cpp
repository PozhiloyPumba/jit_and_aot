#include "passes/constantFoldingPass.hpp"
#include "IRBuilder.hpp"
#include <stdexcept>
#include <iostream>

namespace IRGen {

void ConstantFoldingPass::Run(Function *func) const {
    for (auto* bb : func->GetAllBBs()) {
        for (auto *cur = bb->GetBeginBB(); cur != nullptr;) {
            cur = CollapseInstr(cur);
        }
    }
}

Instruction *ConstantFoldingPass::CollapseInstr(Instruction *instr) const {
	auto op = instr->GetOpcode();
	if (IRGen::IsArithmetic(op)) {
		std::vector<Instruction *> inputs = instr->GetInputs(); // copy because of cleaning orig instruction
		
		bool needOptimize = std::all_of(inputs.begin(), inputs.end(), [](auto *instr){ return instr->GetOpcode() == Opcode::MOVI; });

		if(!needOptimize)
			return instr->GetNextInstr();

		ImmType res;
		if (IRGen::IsTwoValArithmetic(op)) {
			res = Calculate(static_cast<ImmToValInstr *>(inputs[0])->GetImm(), static_cast<ImmToValInstr *>(inputs[1])->GetImm(), op);
		}
		else {
			res = Calculate(static_cast<ImmToValInstr *>(inputs[0])->GetImm(), static_cast<ValAndImmInstr *>(instr)->GetImm(), op);
		}

		auto *bb = instr->GetBB();

		auto &builder = InstructionBuilder::GetInstance();
		auto *subs = builder.BuildMovI(res);
		for (auto &user : instr->GetUsers()) {
			auto &inputs = user->GetInputs();
			std::replace(inputs.begin(), inputs.end(), static_cast<Instruction *>(instr),
						 static_cast<Instruction *>(subs));
		}
		bb->ReplaceInstr(instr, {subs});
		for(auto &input: inputs) {
			auto *inputBB = input->GetBB();
			if (input->GetUsers().empty() && inputBB) {
				inputBB->AssasinateInstr(input);
			}
		}
		return subs;
	}

	return instr->GetNextInstr();
}

ImmType ConstantFoldingPass::Calculate(ImmType imm1, ImmType imm2, Opcode op) const {
	ImmType res;
	assert(imm1.index() == imm2.index());
	std::visit([&res, imm2, op](auto first){
	std::visit([&res, first, op](auto second){
		using T = std::decay_t<decltype(first)>;
		switch (op) {
			case Opcode::MUL:	res = static_cast<T>(first * second);	break;
			case Opcode::ADD:	res = static_cast<T>(first + second);	break;
			case Opcode::SUB:	res = static_cast<T>(first - second);	break;
			case Opcode::SHR:	res = static_cast<T>(first >> second);	break;
			case Opcode::AND:	res = static_cast<T>(first & second);	break;
			case Opcode::ADDI:	res = static_cast<T>(first + second);	break;
			case Opcode::SUBI:	res = static_cast<T>(first - second);	break;
			case Opcode::SHRI:	res = static_cast<T>(first >> second);	break;
			case Opcode::SHLI:	res = static_cast<T>(first << second);	break;
			case Opcode::ANDI:	res = static_cast<T>(first & second);	break;
			case Opcode::DEC:	res = static_cast<T>(first - second);	break;
			default:
				throw std::runtime_error("Wrong opcode "s + IRGen::toString(op) + " " + CUR_FUNC_NAME);
		}
		
	}, imm2);
	}, imm1);

	return res;
}

}