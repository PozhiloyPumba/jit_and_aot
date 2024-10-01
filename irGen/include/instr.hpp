#ifndef __INSTR_HPP__
#define __INSTR_HPP__

#include "opcodes.hpp"
#include "instrTypes.hpp"
#include "helpers/defines.hpp"

namespace IRGen {
class BB;

class Instruction {
protected:
    Opcode opcode_;
    InstrType instrType_;
private:
    Instruction *prev_;
    Instruction *next_;
    BB *parentBB_;

public:
    Instruction(Opcode opcode, InstrType type)
        : opcode_(opcode), instrType_(type), prev_(nullptr), next_(nullptr), parentBB_(nullptr) {}

    NO_COPY_NO_MOVE(Instruction);
    virtual ~Instruction() = default;

    inline Instruction *GetPrevInstr() const { return prev_; }
    inline Instruction *GetNextInstr() const { return next_; }
    inline BB *GetBB() const { return parentBB_; }
    inline Opcode GetOpcode() const { return opcode_; }

    inline void SetPrevInstr(Instruction *instr) { prev_ = instr; }
    inline void SetNextInstr(Instruction *instr) { next_ = instr; }
    inline void SetBB(BB *bb) { parentBB_ = bb; }
    void Euthanasia();
	// I am anchor
    void InsertBefore(Instruction *toInsert);
	// I am anchor
    void InsertAfter(Instruction *toInsert);

	virtual std::string toString() { return IRGen::toString(opcode_) + std::string(".") + IRGen::toString(instrType_); };
	std::string RegToString() { return IRGen::typeToReg(instrType_);}
};

}

#endif