#ifndef __FUNCTION_HPP__
#define __FUNCTION_HPP__

#include "BB.hpp"
#include "helpers/defines.hpp"
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace IRGen {

// For func
class ParameterInstr;

class Function {
private:
    BB *firstBB_;
    BB *lastBB_;
    std::list<BB *> BBs_;
    std::string name_;
    std::vector<ParameterInstr *> params_;
    InstrType retType_;
    std::unordered_map<BB *, std::unordered_set<BB *>> domTree_;
    std::unordered_map<BB *, BB *> domTreeParents_;
    bool dTreeBuild = false;

public:
    Function() : firstBB_(nullptr), lastBB_(nullptr) {}

    NO_COPY_NO_MOVE(Function);
    ~Function() = default;

    CONST_AND_NON_CONST_GETTER(GetFirstBB, firstBB_);
    CONST_AND_NON_CONST_GETTER(GetLastBB, lastBB_);

    inline void SetFirstBB(BB *bb) {
        firstBB_ = bb;
    }
    inline void SetLastBB(BB *bb) {
        lastBB_ = bb;
    }
    inline void SetName(const std::string &name) {
        name_ = name;
    }
    inline void SetRetType(InstrType retType) {
        retType_ = retType;
    }
    inline void SetParams(const std::vector<ParameterInstr *> &params) {
        params_ = params;
    }
    inline ParameterInstr *GetParam(size_t i) {
        return params_[i];
    }

    void AddBB(BB *bb);
    void AssasinateBB(BB *bb);
    void AddBBAsPred(BB *newBB, BB *anchor, bool cond);
    void AddBBAsSucc(BB *newBB, BB *anchor, bool cond);
    void DeletePreds(BB *bb);
    void DeleteSuccs(BB *bb);
    bool DominatedBy(BB *child, BB *parent) const;
    void dump() const;
    std::list<BB *> &GetAllBBs() {
        return BBs_;
    }
    std::unordered_map<BB *, std::unordered_set<BB *>> &buildDominatorTree();
};

} // namespace IRGen

#endif