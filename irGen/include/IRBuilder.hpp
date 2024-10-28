#ifndef __IRBUILDER_HPP__
#define __IRBUILDER_HPP__

#include "instrDef.hpp"
#include <vector>
#include "helpers/defines.hpp"

namespace IRGen {

class IRGenerator {
  public:
    IRGenerator() {}
    ~IRGenerator() { Clear(); }

  public:
    Function *CreateFunction(const std::string &name, InstrType retType, const std::vector<ParameterInstr *> &params);
    BB *CreateEmptyBB(Function *);
    void Clear();

  private:
    std::vector<BB *> BBs_;
    std::vector<Function *> graph_;
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
	ImmToValInstr<T> *BuildMovI(T imm) {
		auto *inst = new ImmToValInstr(Opcode::MOVI, GetInstrType<T>(), imm);
        instructions_.push_back(inst);
        return inst;
	}
	
	TwoValInstr *BuildMul(InstrType type, Instruction *first, Instruction *second) {
		auto *inst = new TwoValInstr(Opcode::MUL, type, first, second);
        instructions_.push_back(inst);
        return inst;
	}

	template<typename T>
	ValAndImmInstr<T> *BuildAddI(Instruction *val, T imm) {
		auto *inst = new ValAndImmInstr(Opcode::ADDI, GetInstrType<T>(), val, imm);
        instructions_.push_back(inst);
        return inst;
	}

	CmpInstr *BuildCmp(InstrType type, Instruction *first, Instruction *second) {
		auto *inst = new CmpInstr(Opcode::CMP, type, first, second);
        instructions_.push_back(inst);
        return inst;
	}

	JumpInstr *BuildJump(BB *dest) {
		auto *inst = new JumpInstr(Opcode::JMP, dest);
        instructions_.push_back(inst);
        return inst;
	}

	JumpInstr *BuildJa(BB *trueJump, Instruction *predicate, BB *falseJump) {
		auto *inst = new JumpInstr(Opcode::JA, trueJump, predicate, falseJump);
        instructions_.push_back(inst);
        return inst;
	}
	OneValInstr *BuildRet(InstrType type, Instruction *val) {
		auto *inst = new OneValInstr(Opcode::RET, type, val);
        instructions_.push_back(inst);
        return inst;
	}
	
	CastInstr *BuildCast(InstrType type, Instruction *from) {
		auto *inst = new CastInstr(Opcode::CAST, type, from);
        instructions_.push_back(inst);
        return inst;
	}

    PhiInstr *BuildPhi(InstrType type) {
		auto *inst = new PhiInstr(type);
        instructions_.push_back(inst);
        return inst;
	}

    ParameterInstr *BuildParam(InstrType type) {
		auto *inst = new ParameterInstr(type);
        instructions_.push_back(inst);
        return inst;
	}
};
}
#endif