#ifndef __INSTRDEF_HPP__
#define __INSTRDEF_HPP__

#include <string>
#include <cassert> 
#include <vector>
#include "instr.hpp"
#include "BB.hpp"

namespace IRGen {

using namespace std::literals;

// parameters of function is parameter instruction
class ParameterInstr: public Instruction {
public:
	ParameterInstr(InstrType type, bool setId = true): Instruction(Opcode::PARAM, type, setId) {
	}

	inline std::string toString() const override {
		return IRGen::toString(instrType_) + " @"s + std::to_string(instrId_);
	}
};


template<typename T>
class ImmToValInstr: public Instruction {
private:
	T imm_;
public:
	ImmToValInstr(Opcode op, InstrType type, T imm, bool setId = true): Instruction(op, type, setId), imm_(imm) {}

	inline std::string toString() const override {
		return Instruction::toString() + " @" + std::to_string(instrId_) + " " + std::to_string(imm_);
	}
};

class OneValInstr: public Instruction {
private:
	Instruction *value_;
public:
	OneValInstr(Opcode op, InstrType type, Instruction *val, bool setId = true): Instruction(op, type, setId), value_(val) {
	}

	inline std::string toString() const override  {
		return Instruction::toString() + " @" + std::to_string(value_->GetId());
	}
};

class CastInstr: public Instruction {
private:
	Instruction *from_;
public:
	CastInstr(Opcode op, InstrType type, Instruction *from, bool setId = true): Instruction(op, type, setId), from_(from) {
		assert(from);
	}

	inline std::string toString() const override  {
		return Instruction::toString() + "from" + IRGen::toString(from_->GetType()) + " @" + std::to_string(instrId_) + 
			" @" + std::to_string(from_->GetId());
	}
};

class CmpInstr: public Instruction {
private:
	Instruction *first_;
	Instruction *second_;
public:
	CmpInstr(Opcode op, InstrType type, Instruction *first, Instruction *second, bool setId = true): 
		Instruction(op, type, setId), first_(first), second_(second) {
	}

	inline std::string toString() const override  {
		return Instruction::toString() + " @" + std::to_string(instrId_) + " @" + std::to_string(first_->GetId()) + " @" + std::to_string(second_->GetId());
	}
};


class TwoValInstr: public Instruction {
private:
	Instruction *first_;
	Instruction *second_;
public:
	TwoValInstr(Opcode op, InstrType type, Instruction *first, Instruction *second, bool setId = true): 
		Instruction(op, type, setId), 
		first_(first), second_(second) {
		assert(first && second);
	}
	inline std::string toString() const override  {
		return Instruction::toString() + " @" + std::to_string(instrId_) + " @" + std::to_string(first_->GetId()) + 
				" @" + std::to_string(second_->GetId());
	}
};

template<typename T>
class ValAndImmInstr: public Instruction {
private:
	Instruction *value_;
	T imm_;
public:
	ValAndImmInstr(Opcode op, InstrType type, Instruction *val, T imm, bool setId = true): Instruction(op, type, setId), 
		value_(val), imm_(imm) {
	}
	
	inline std::string toString() const override  {
		return Instruction::toString() + " @" + std::to_string(instrId_) + " @" + std::to_string(value_->GetId()) + " " + std::to_string(imm_);
	}
};

class JumpInstr: public Instruction {
private:
	BB *trueOut_;
	BB *falseOut_;
	Instruction *pred_;
public:
	JumpInstr(Opcode op, BB *trueJump, Instruction *pred = nullptr, BB *falseJump = nullptr, bool setId = true): Instruction(op, InstrType::VOID, setId), 
		trueOut_(trueJump), falseOut_(falseJump), pred_(pred) {
		assert(trueOut_);
		assert(!falseOut_ == !pred);
	}

	inline std::string toString() const override  {
		auto falseStr = falseOut_ ? " %" + std::to_string(falseOut_->GetId()) : "";
		auto predStr = falseOut_ ? " @" + std::to_string(pred_->GetId()) + " %": " %";
		return IRGen::toString(opcode_) + predStr + std::to_string(trueOut_->GetId()) + falseStr;
	}
};

class PhiInstr: public Instruction {
private:
	std::vector<std::pair<BB *, Instruction *>> preds_;

public:
	PhiInstr(InstrType t, bool setId = true): Instruction(Opcode::PHI, t, setId) {
	}

	inline void SetIncoming(BB *pred, Instruction *val) {
		preds_.push_back({pred, val});
	}

	inline std::string toString() const override  {
		std::string s;
		const char *padding = " ";
		for(const auto &p: preds_) {
			s += padding + "[@"s + std::to_string(p.second->GetId()) + " %" + std::to_string(p.first->GetId()) +  "]";
			padding = ", ";
		}
		return Instruction::toString() + " @" + std::to_string(instrId_) + s;
	}
};

}


#endif