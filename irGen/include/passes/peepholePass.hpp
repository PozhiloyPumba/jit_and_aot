#ifndef __PEEPHOLE_PASS_HPP__
#define __PEEPHOLE_PASS_HPP__

#include "instrDef.hpp"
#include "pass.hpp"
#include <string_view>

namespace IRGen {
class PeepHolePass : public Pass {
    constexpr static inline const char *PeepHolePassName = "PeepHolePass";

    Instruction *SubPeephole(TwoValInstr *instr) const;
    Instruction *ShrPeephole(TwoValInstr *instr) const;
    Instruction *AndPeephole(TwoValInstr *instr) const;
    Instruction *SubIPeephole(ValAndImmInstr *instr) const;
    Instruction *ShrIPeephole(ValAndImmInstr *instr) const;
    Instruction *AndIPeephole(ValAndImmInstr *instr) const;

public:
    PeepHolePass() : Pass(PeepHolePassName) {}
    void Run(Function *func) const override;
};

} // namespace IRGen

#endif