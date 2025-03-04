#ifndef __PEEPHOLE_PASS_HPP__
#define __PEEPHOLE_PASS_HPP__

#include "instrDef.hpp"
#include "pass.hpp"

namespace IRGen {
class ConstantFoldingPass : public Pass {
    constexpr static inline const char *ConstantFoldingPassName = "ConstantFoldingPass";

	Instruction *CollapseInstr(Instruction *instr) const;
	ImmType Calculate(ImmType imm1, ImmType imm2, Opcode op) const;

public:
	ConstantFoldingPass() : Pass(ConstantFoldingPassName) {}
    void Run(Function *func) const override;
};

} // namespace IRGen

#endif