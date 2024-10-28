#ifndef __INSTRDEF_HPP__
#define __INSTRDEF_HPP__

#include <string>
#include "instr.hpp"
#include "BB.hpp"

namespace IRGen {

using namespace std::literals;

template<typename T>
class RegImmInstr: public Instruction {
private:
	int reg_;
	T imm_;
public:
	RegImmInstr(Opcode op, InstrType type, int reg, T imm, bool setId = true): Instruction(op, type, setId), reg_(reg), imm_(imm) {
	}

	std::string toString() override {
		return Instruction::toString() + " " + RegToString() + std::to_string(reg_) + " " + std::to_string(imm_);
	}
};

class OneRegInstr: public Instruction {
private:
	int reg_;
public:
	OneRegInstr(Opcode op, InstrType type, int reg, bool setId = true): Instruction(op, type, setId), reg_(reg) {
	}

	std::string toString() override {
		return Instruction::toString() + " " + RegToString() + std::to_string(reg_);
	}
};


class CastInstr: public Instruction {
private:
	int reg1_;
	int reg2_;
	InstrType type2_;
public:
	CastInstr(Opcode op, InstrType type1, int reg1, InstrType type2, int reg2, bool setId = true): Instruction(op, type1, setId), reg1_(reg1)
	, reg2_(reg2), type2_(type2) {
	}

	std::string toString() override {
		return Instruction::toString() + "from" + IRGen::toString(type2_) + " " + RegToString() + std::to_string(reg1_) + 
			" " + IRGen::typeToReg(type2_) + std::to_string(reg2_);
	}
};

class TwoRegInstr: public Instruction {
private:
	int reg1_;
	int reg2_;
public:
	TwoRegInstr(Opcode op, InstrType type, int reg1, int reg2, bool setId = true): Instruction(op, type, setId), reg1_(reg1), reg2_(reg2) {
	}
	std::string toString() override {
		return Instruction::toString() + " " + RegToString() + std::to_string(reg1_) + " " + RegToString() + std::to_string(reg2_);
	}
};


class ThreeRegInstr: public Instruction {
private:
	int reg1_;
	int reg2_;
	int reg3_;
public:
	ThreeRegInstr(Opcode op, InstrType type, int reg1, int reg2, int reg3, bool setId = true): Instruction(op, type, setId), 
		reg1_(reg1), reg2_(reg2), reg3_(reg3) {
	}
	std::string toString() override {
		return Instruction::toString() + " " + RegToString() + std::to_string(reg1_) + " " + RegToString() + std::to_string(reg2_) +" " + RegToString() + std::to_string(reg3_);
	}
};

template<typename T>
class TwoRegImmInstr: public Instruction {
private:
	int reg1_;
	int reg2_;
	T imm_;
public:
	TwoRegImmInstr(Opcode op, InstrType type, int reg1, int reg2, T imm, bool setId = true): Instruction(op, type, setId), 
		reg1_(reg1), reg2_(reg2), imm_(imm) {
	}
	
	std::string toString() override {
		return Instruction::toString() + " " + RegToString() + std::to_string(reg1_) + " " + RegToString() + std::to_string(reg2_) +" " + std::to_string(imm_);
	}
};

class JumpInstr: public Instruction {
private:
	BB *out_;
public:
	JumpInstr(Opcode op, BB *out, bool setId = true): Instruction(op, InstrType::VOID, setId), out_(out){
	}

	std::string toString() override {
		return IRGen::toString(opcode_) + " "s + std::to_string(out_->GetId());
	}
};

}


#endif