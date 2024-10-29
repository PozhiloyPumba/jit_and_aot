#ifndef __LOOP_HPP__
#define __LOOP_HPP__

#include "BB.hpp"
#include <helpers/defines.hpp>
#include <unordered_set>
#include <vector>

namespace IRGen {

class Loop {
    BB *header_;
    std::unordered_set<BB *> latches_;
    std::unordered_set<BB *> body_;
    Loop *outerLoop_;
    std::unordered_set<Loop *> innerLoops_;
    bool isReducible_ = true;

public:
    CONST_AND_NON_CONST_GETTER(GetLatches, latches_);
    CONST_AND_NON_CONST_GETTER(GetBody, body_);
    CONST_AND_NON_CONST_GETTER(GetHeader, header_);
    inline void SetHeader(BB *h) {
        header_ = h;
    }
    inline void SetReducible(bool val) {
        isReducible_ &= val;
    }
    inline bool GetReducible() const {
        return isReducible_;
    }

    inline Loop *GetOuterLoop() const {
        return outerLoop_;
    }
    inline void SetOuterLoop(Loop *l) {
        outerLoop_ = l;
    }

    inline std::unordered_set<Loop *> GetInnerLoops() const {
        return innerLoops_;
    }
    inline void AddInnerLoop(Loop *l) {
        innerLoops_.insert(l);
    }
    inline void RemoveInnerLoop(Loop *l) {
        innerLoops_.erase(l);
    }
};

} // namespace IRGen

#endif