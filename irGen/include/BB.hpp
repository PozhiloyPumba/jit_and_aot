#ifndef __BB_HPP__
#define __BB_HPP__

#include "helpers/defines.hpp"
#include "instr.hpp"
#include "loop.hpp"
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

    Loop *loop_ = nullptr;

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

    inline uint64_t GetId() const {
        return bbId_;
    }
    CONST_AND_NON_CONST_GETTER(GetPreds, preds_);
    CONST_AND_NON_CONST_GETTER(GetSuccs, succs_);
    CONST_AND_NON_CONST_GETTER(GetBeginBB, beginBB_);
    CONST_AND_NON_CONST_GETTER(GetEndBB, endBB_);
    CONST_AND_NON_CONST_GETTER(GetFunction, graph_);
    CONST_AND_NON_CONST_GETTER(GetLoop, loop_);

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
    bool DominatedBy(BB *other);
    BB *GetIDom();
    bool IsLoopHeader() const;
    void dump() const;
};

} // namespace IRGen

#endif