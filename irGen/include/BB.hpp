#ifndef __BB_HPP__
#define __BB_HPP__

#include <list>
#include "helpers/defines.hpp"
#include "instr.hpp"

namespace IRGen {
class Graph;

class BB {
public:
	constexpr static const uint64_t INVALID_BB_ID = 0xDEADBEEFDEADBEEF;
private:
    uint64_t bbId_ = INVALID_BB_ID;
    std::list<BB *> preds_;
    std::list<BB *> succs_;
    Instruction *beginBB_;
    Instruction *endBB_;
    Graph *graph_;
public:
    BB(): bbId_(INVALID_BB_ID), 
		beginBB_(nullptr), endBB_(nullptr), graph_(nullptr){};
    explicit BB(Graph *graph)
        : bbId_(INVALID_BB_ID), 
		beginBB_(nullptr), endBB_(nullptr), graph_(graph){};

    NO_COPY_NO_MOVE(BB);
    ~BB() = default;

    inline uint64_t GetId() { return bbId_; }
    inline std::list<BB *> &GetPreds() { return preds_; }
    inline std::list<BB *> &GetSuccs() { return succs_; }
    inline Instruction *GetBeginBB() { return beginBB_; }
    inline Instruction *GetEndBB() { return endBB_; }
    inline Graph *GetGraph() { return graph_; }

public:
    void SetId(uint64_t id) { bbId_ = id; }

    void AddPred(BB *bb);
    void DeletePred(BB *bb);
    void AddSucc(BB *bb);
    void DeleteSucc(BB *bb);

    void InsertInstrBefore(Instruction *insert, Instruction *anchor);
    void InsertInstrAfter(Instruction *insert, Instruction *anchor);
    void SetGraph(Graph *newGraph) { graph_ = newGraph; }
    void AssasinateInstr(Instruction *instr);
    void AddInstrForward(Instruction *instr);
    void AddInstrBackward(Instruction *instr);
	void dump();
};

}


#endif