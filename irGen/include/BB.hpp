#ifndef __BB_HPP__
#define __BB_HPP__

#include "helpers/defines.hpp"
#include "instr.hpp"
#include <array>
#include <cstdint>
#include <list>
#include <string>

namespace IRGen {
class Function;

class BB {
public:
    constexpr static const uint64_t INVALID_BB_ID = 0xDEADBEEFDEADBEEF;

private:
    static inline uint64_t nextBB_id = 0;
    uint64_t bbId_ = INVALID_BB_ID;
    std::list<BB *> preds_;
    std::array<BB *, 2> succs_ = {nullptr, nullptr}; // false, true paths
    Instruction *beginBB_;
    Instruction *endBB_;
    Function *graph_;
    inline void SetDefaultId() {
        bbId_ = nextBB_id++;
    }

public:
    BB(bool setId = true) : beginBB_(nullptr), endBB_(nullptr), graph_(nullptr) {
        if (setId)
            SetDefaultId();
        else
            bbId_ = INVALID_BB_ID;
    }
    explicit BB(Function *graph, bool setId = true) : beginBB_(nullptr), endBB_(nullptr), graph_(graph) {
        if (setId)
            SetDefaultId();
        else
            bbId_ = INVALID_BB_ID;
    }

    NO_COPY_NO_MOVE(BB);
    ~BB() = default;

    inline uint64_t GetId() {
        return bbId_;
    }
    inline std::list<BB *> &GetPreds() {
        return preds_;
    }
    inline std::array<BB *, 2> &GetSuccs() {
        return succs_;
    }
    inline Instruction *GetBeginBB() {
        return beginBB_;
    }
    inline Instruction *GetEndBB() {
        return endBB_;
    }
    inline Function *GetFunction() {
        return graph_;
    }

public:
    inline void SetId(uint64_t id) {
        bbId_ = id;
    }
    void AddPred(BB *bb);
    void DeletePred(BB *bb);
    void AddSucc(BB *bb, bool cond);
    void DeleteSucc(BB *bb);

    void InsertInstrBefore(Instruction *insert, Instruction *anchor);
    void InsertInstrAfter(Instruction *insert, Instruction *anchor);
    void SetFunction(Function *newFunction) {
        graph_ = newFunction;
    }
    void AssasinateInstr(Instruction *instr);
    void AddInstrForward(Instruction *instr);
    void AddInstrBackward(Instruction *instr);
    void dump() const;
};

} // namespace IRGen

#endif