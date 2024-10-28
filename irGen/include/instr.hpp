#ifndef __INSTR_HPP__
#define __INSTR_HPP__

#include "helpers/defines.hpp"
#include "instrTypes.hpp"
#include "opcodes.hpp"
#include <cstdint>
#include <string>

namespace IRGen {
class BB;

class Instruction {
protected:
    Opcode opcode_;
    InstrType instrType_;
    uint64_t instrId_;

private:
    Instruction *prev_;
    Instruction *next_;
    BB *parentBB_;
    static inline uint64_t nextInstr_id = 0;
    inline void SetDefaultId() {
        instrId_ = nextInstr_id++;
    }

public:
    constexpr static const uint64_t INVALID_INSTR_ID = 0xDEADBEEFDEADBEEF;

    Instruction(Opcode opcode, InstrType type, bool setId = true)
        : opcode_(opcode), instrType_(type), prev_(nullptr), next_(nullptr), parentBB_(nullptr) {

        if (setId)
            SetDefaultId();
        else
            instrId_ = INVALID_INSTR_ID;
    }

    NO_COPY_NO_MOVE(Instruction);
    virtual ~Instruction() = default;

    inline Instruction *GetPrevInstr() const {
        return prev_;
    }
    inline Instruction *GetNextInstr() const {
        return next_;
    }
    inline BB *GetBB() const {
        return parentBB_;
    }
    inline Opcode GetOpcode() const {
        return opcode_;
    }
    inline uint64_t GetId() const {
        return instrId_;
    }
    inline InstrType GetType() const {
        return instrType_;
    }

    inline void SetPrevInstr(Instruction *instr) {
        prev_ = instr;
    }
    inline void SetNextInstr(Instruction *instr) {
        next_ = instr;
    }
    inline void SetBB(BB *bb) {
        parentBB_ = bb;
    }
    inline void SetId(uint64_t id) {
        instrId_ = id;
    }

    void Euthanasia();
    // I am anchor
    void InsertBefore(Instruction *toInsert);
    // I am anchor
    void InsertAfter(Instruction *toInsert);

    virtual std::string toString() const {
        return IRGen::toString(opcode_) + std::string(".") + IRGen::toString(instrType_);
    };
};

} // namespace IRGen

#endif