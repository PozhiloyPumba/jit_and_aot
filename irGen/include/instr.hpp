#ifndef __INSTR_HPP__
#define __INSTR_HPP__

#include "helpers/defines.hpp"
#include "instrTypes.hpp"
#include "liveRange.hpp"
#include "opcodes.hpp"
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

namespace IRGen {
class BB;

class Instruction {
protected:
    Opcode opcode_;
    InstrType instrType_;
    uint64_t instrId_;

    uint64_t linearNumber_{0};
    uint64_t liveNumber_{0};

    LiveRange live_;
    std::vector<Instruction *> preds_;
    std::unordered_set<Instruction *> users_; // users of this instruction

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

    CONST_AND_NON_CONST_GETTER(GetLiveNumber, liveNumber_);
    CONST_AND_NON_CONST_GETTER(GetLinearNumber, linearNumber_);
    CONST_AND_NON_CONST_GETTER(GetLiveRange, live_);
    CONST_AND_NON_CONST_GETTER(GetInputs, preds_);
    CONST_AND_NON_CONST_GETTER(GetUsers, users_);

    inline void AddUser(Instruction *instr) {
        users_.insert(instr);
    }

    inline void RemoveUser(Instruction *instr) {
        users_.erase(instr);
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

    bool IsPhiInstr() const {
        return opcode_ == Opcode::PHI;
    }

    void Euthanasia();
    // I am anchor
    void InsertBefore(Instruction *toInsert);
    // I am anchor
    void InsertAfter(Instruction *toInsert);

    bool DominatedBy(Instruction *other);

    virtual std::string toString() const {
        return IRGen::toString(opcode_) + std::string(".") + IRGen::toString(instrType_);
    };
};

} // namespace IRGen

#endif