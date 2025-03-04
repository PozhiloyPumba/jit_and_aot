#ifndef __INSTRDEF_HPP__
#define __INSTRDEF_HPP__

#include "BB.hpp"
#include "instr.hpp"
#include <cassert>
#include <string>
#include <vector>

namespace IRGen {

using namespace std::literals;

// parameters of function is parameter instruction
class ParameterInstr : public Instruction {
public:
    ParameterInstr(InstrType type, bool setId = true) : Instruction(Opcode::PARAM, type, setId) {}

    inline std::string toString() const override {
        return IRGen::toString(instrType_) + " @"s + std::to_string(instrId_);
    }
};

class ImmToValInstr : public Instruction {
private:
    ImmType imm_;

public:
    ImmToValInstr(Opcode op, InstrType type, ImmType imm, bool setId = true) : Instruction(op, type, setId), imm_(imm) {}

    inline std::string toString() const override {
        return Instruction::toString() + " @" + std::to_string(instrId_) + " " + IRGen::toString(instrType_, imm_);
    }
    const ImmType &GetImm() const {
        return imm_;
    }
};

class OneValInstr : public Instruction {
public:
    OneValInstr(Opcode op, InstrType type, Instruction *val, bool setId = true) : Instruction(op, type, setId) {
        if (op == Opcode::RET && !val)
            return;

        assert(val);
        val->AddUser(this);
        preds_.push_back(val);
    }

    inline std::string toString() const override {
        if (opcode_ == Opcode::RET && preds_.empty())
            return Instruction::toString();
        return Instruction::toString() + " @" + std::to_string(preds_[0]->GetId());
    }
};

class CastInstr : public Instruction {
public:
    CastInstr(Opcode op, InstrType type, Instruction *from, bool setId = true) : Instruction(op, type, setId) {
        assert(from);
        from->AddUser(this);
        preds_.push_back(from);
    }

    inline std::string toString() const override {
        return Instruction::toString() + "from" + IRGen::toString(preds_[0]->GetType()) + " @" + std::to_string(instrId_) + " @" +
               std::to_string(preds_[0]->GetId());
    }
};

class CmpInstr : public Instruction {
public:
    CmpInstr(Opcode op, InstrType type, Instruction *first, Instruction *second, bool setId = true)
        : Instruction(op, type, setId) {
        first->AddUser(this);
        second->AddUser(this);
        preds_.push_back(first);
        preds_.push_back(second);
    }

    inline std::string toString() const override {
        return Instruction::toString() + " @" + std::to_string(instrId_) + " @" + std::to_string(preds_[0]->GetId()) + " @" +
               std::to_string(preds_[1]->GetId());
    }
};

class TwoValInstr : public Instruction {
public:
    TwoValInstr(Opcode op, InstrType type, Instruction *first, Instruction *second, bool setId = true)
        : Instruction(op, type, setId) {
        assert(first && second);
        first->AddUser(this);
        second->AddUser(this);
        preds_.push_back(first);
        preds_.push_back(second);
    }
    inline std::string toString() const override {
        return Instruction::toString() + " @" + std::to_string(instrId_) + " @" + std::to_string(preds_[0]->GetId()) + " @" +
               std::to_string(preds_[1]->GetId());
    }
};

class ValAndImmInstr : public Instruction {
private:
    ImmType imm_;

public:
    ValAndImmInstr(Opcode op, InstrType type, Instruction *val, ImmType imm, bool setId = true)
        : Instruction(op, type, setId), imm_(imm) {
        assert(val);
        val->AddUser(this);
        preds_.push_back(val);
    }

    inline std::string toString() const override {
        return Instruction::toString() + " @" + std::to_string(instrId_) + " @" + std::to_string(preds_[0]->GetId()) + " " +
               IRGen::toString(instrType_, imm_);
    }
    const ImmType &GetImm() const {
        return imm_;
    }
};

class JumpInstr : public Instruction {
private:
    BB *trueOut_;
    BB *falseOut_;

public:
    JumpInstr(Opcode op, BB *trueJump, Instruction *pred = nullptr, BB *falseJump = nullptr, bool setId = true)
        : Instruction(op, InstrType::VOID, setId), trueOut_(trueJump), falseOut_(falseJump) {
        assert(trueOut_);
        assert(!falseOut_ == !pred);
        if (pred) { // for jump instr
            pred->AddUser(this);
            preds_.push_back(pred);
        }
    }

    inline std::string toString() const override {
        auto falseStr = falseOut_ ? " %" + std::to_string(falseOut_->GetId()) : "";
        auto predStr = falseOut_ ? " @" + std::to_string(preds_[0]->GetId()) + " %" : " %";
        return IRGen::toString(opcode_) + predStr + std::to_string(trueOut_->GetId()) + falseStr;
    }
};

class PhiInstr : public Instruction {
public:
    PhiInstr(InstrType t, bool setId = true) : Instruction(Opcode::PHI, t, setId) {}

    inline void SetIncoming(Instruction *val) {
        assert(val);
        val->AddUser(this);
        preds_.push_back(val);
    }

    inline std::string toString() const override {
        std::string s;
        const char *padding = " ";
        for (const auto &p : preds_) {
            s += padding + "[@"s + std::to_string(p->GetId()) + " %" + std::to_string(p->GetBB()->GetId()) + "]";
            padding = ", ";
        }
        return Instruction::toString() + " @" + std::to_string(instrId_) + s;
    }
};

} // namespace IRGen

#endif