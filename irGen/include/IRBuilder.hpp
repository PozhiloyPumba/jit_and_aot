#ifndef __IRBUILDER_HPP__
#define __IRBUILDER_HPP__

#include "helpers/defines.hpp"
#include "instrDef.hpp"
#include <vector>

namespace IRGen {

class IRGenerator {
    IRGenerator() {}
    std::vector<BB *> BBs_;
    std::vector<Function *> graph_;

public:
    static IRGenerator &GetInstance() {
        static IRGenerator instance;
        return instance;
    }

    NO_COPY_NO_MOVE(IRGenerator);
    ~IRGenerator() {
        Clear();
    }

    Function *CreateFunction(const std::string &name, InstrType retType, const std::vector<ParameterInstr *> &params);
    BB *CreateEmptyBB(Function *);
    void Clear();
};

class InstructionBuilder final {
private:
    std::vector<Instruction *> instructions_;
    InstructionBuilder() = default;

public:
    static InstructionBuilder &GetInstance() {
        static InstructionBuilder instance;
        return instance;
    }

    NO_COPY_NO_MOVE(InstructionBuilder);

    ~InstructionBuilder() {
        Clear();
    }

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

    ImmToValInstr *BuildMovI(ImmType imm) {
        auto *inst = new ImmToValInstr(Opcode::MOVI, GetEnumFromVariant(imm), imm);
        instructions_.push_back(inst);
        return inst;
    }

    TwoValInstr *BuildMul(InstrType type, Instruction *first, Instruction *second) {
        auto *inst = new TwoValInstr(Opcode::MUL, type, first, second);
        instructions_.push_back(inst);
        return inst;
    }

    TwoValInstr *BuildAdd(InstrType type, Instruction *first, Instruction *second) {
        auto *inst = new TwoValInstr(Opcode::ADD, type, first, second);
        instructions_.push_back(inst);
        return inst;
    }

    TwoValInstr *BuildSub(InstrType type, Instruction *first, Instruction *second) {
        auto *inst = new TwoValInstr(Opcode::SUB, type, first, second);
        instructions_.push_back(inst);
        return inst;
    }

    TwoValInstr *BuildShr(InstrType type, Instruction *first, Instruction *second) {
        auto *inst = new TwoValInstr(Opcode::SHR, type, first, second);
        instructions_.push_back(inst);
        return inst;
    }

    TwoValInstr *BuildAnd(InstrType type, Instruction *first, Instruction *second) {
        auto *inst = new TwoValInstr(Opcode::AND, type, first, second);
        instructions_.push_back(inst);
        return inst;
    }

    ValAndImmInstr *BuildAddI(Instruction *val, ImmType imm) {
        auto *inst = new ValAndImmInstr(Opcode::ADDI, GetEnumFromVariant(imm), val, imm);
        instructions_.push_back(inst);
        return inst;
    }

    ValAndImmInstr *BuildSubI(Instruction *val, ImmType imm) {
        auto *inst = new ValAndImmInstr(Opcode::SUBI, GetEnumFromVariant(imm), val, imm);
        instructions_.push_back(inst);
        return inst;
    }

    ValAndImmInstr *BuildAndI(Instruction *val, ImmType imm) {
        auto *inst = new ValAndImmInstr(Opcode::ANDI, GetEnumFromVariant(imm), val, imm);
        instructions_.push_back(inst);
        return inst;
    }

    ValAndImmInstr *BuildShrI(Instruction *val, ImmType imm) {
        auto *inst = new ValAndImmInstr(Opcode::SHRI, GetEnumFromVariant(imm), val, imm);
        instructions_.push_back(inst);
        return inst;
    }

    ValAndImmInstr *BuildShlI(Instruction *val, ImmType imm) {
        auto *inst = new ValAndImmInstr(Opcode::SHLI, GetEnumFromVariant(imm), val, imm);
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

    OneValInstr *BuildDec(InstrType type, Instruction *val) {
        auto *inst = new OneValInstr(Opcode::DEC, type, val);
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
} // namespace IRGen
#endif