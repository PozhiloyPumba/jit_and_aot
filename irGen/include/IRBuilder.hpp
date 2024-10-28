#ifndef __IRBUILDER_HPP__
#define __IRBUILDER_HPP__

#include "instrDef.hpp"
#include <vector>
#include "helpers/defines.hpp"

namespace IRGen {

class IRGenerator {
  public:
    IRGenerator() : graph_(nullptr) {}
    ~IRGenerator() { Clear(); }

  public:
    void CreateGraph();
    BB *CreateEmptyBB();
    Graph *GetGraph() const { return graph_; }
    void Clear();

  private:
    std::vector<BB *> BBs_;
    Graph *graph_;
};

class InstructionBuilder final {
private:
    std::vector<Instruction *> instructions_;

public:
    InstructionBuilder() = default;
	NO_COPY_NO_MOVE(InstructionBuilder);

    ~InstructionBuilder() { Clear(); }

    inline void PushBackInstr(BB *bb, Instruction *instr) {
        bb->AddInstrBackward(instr);
    }

    inline void PushForwardInst(BB *bb, Instruction *instr) {
        bb->AddInstrForward(instr);
    }

    void Clear() noexcept {
        for (auto *instr : instructions_)
			delete instr;
        instructions_.clear();
    }

	template<typename T>
	RegImmInstr<T> *BuildMovI(int reg, T imm) {
		auto *inst = new RegImmInstr(Opcode::MOVI, GetInstrType<T>(), reg, imm);
        instructions_.push_back(inst);
        return inst;
	}
	
	ThreeRegInstr *BuildMul(InstrType type, int reg1, int reg2, int reg3) {
		auto *inst = new ThreeRegInstr(Opcode::MUL, type, reg1, reg2, reg3);
        instructions_.push_back(inst);
        return inst;
	}

	template<typename T>
	TwoRegImmInstr<T> *BuildAddI(int reg1, int reg2, T imm) {
		auto *inst = new TwoRegImmInstr(Opcode::ADDI, GetInstrType<T>(), reg1, reg2, imm);
        instructions_.push_back(inst);
        return inst;
	}

	TwoRegInstr *BuildCmp(InstrType type, int reg1, int reg2) {
		auto *inst = new TwoRegInstr(Opcode::CMP, type, reg1, reg2);
        instructions_.push_back(inst);
        return inst;
	}

	JumpInstr *BuildJump(BB *dest) {
		auto *inst = new JumpInstr(Opcode::JMP, dest);
        instructions_.push_back(inst);
        return inst;
	}

	JumpInstr *BuildJa(BB *dest) {
		auto *inst = new JumpInstr(Opcode::JA, dest);
        instructions_.push_back(inst);
        return inst;
	}
	OneRegInstr *BuildRet(InstrType type, int reg) {
		auto *inst = new OneRegInstr(Opcode::RET, type, reg);
        instructions_.push_back(inst);
        return inst;
	}
	
	CastInstr *BuildCast(InstrType type1, int reg1, InstrType type2, int reg2) {
		auto *inst = new CastInstr(Opcode::CAST, type1, reg1, type2, reg2);
        instructions_.push_back(inst);
        return inst;
	}
};
}
#endif