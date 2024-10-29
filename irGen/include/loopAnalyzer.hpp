#ifndef __LOOP_ANALYZER_HPP__
#define __LOOP_ANALYZER_HPP__

#include "BB.hpp"
#include "function.hpp"
#include "loop.hpp"
#include <cassert>
#include <helpers/defines.hpp>
#include <unordered_set>
#include <vector>

namespace IRGen {

class LoopAnalyzer {

    Function *graph_;
    std::unordered_map<BB *, Loop *> loops_;
    inline Loop *CreateLoop(BB *header) {
        Loop *l = new Loop();
        l->SetHeader(header);
        loops_.insert({header, l});
        return l;
    }

    enum Color { WHITE = 0, BLACK = 1, GREY = 1 << 1 };
    std::unordered_map<BB *, unsigned> markers_;
    void Dfs(BB *cur);
    void AddSourcesFromLatch(BB *latch, Loop *loop);

    Loop *root_;

public:
    LoopAnalyzer(Function *graph) : graph_(graph) {
        assert(graph_);
    }
    ~LoopAnalyzer();
    void BuildLoopTree();
    inline const Loop *GetRootLoop() const {
        return root_;
    }
    inline auto &GetLoops() const {
        return loops_;
    }
};

} // namespace IRGen

#endif