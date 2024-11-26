#ifndef __LIVENESS_ANALYSIS_HPP__
#define __LIVENESS_ANALYSIS_HPP__

#include "BB.hpp"
#include "function.hpp"
#include "loopAnalyzer.hpp"
#include <cassert>
#include <iostream>
#include <stack>
#include <vector>

namespace IRGen {

class LivenessAnalysis {

    Function *graph_;

    LoopAnalyzer loopTree_;

    template <typename T> using BBmapT = std::unordered_map<BB *, T>;

    void SCCUtil(BB *u, BBmapT<int> &disc, BBmapT<int> &low, std::stack<BB *> &st, BBmapT<bool> &stackMember,
                 std::vector<BB *> &res) const;
    std::vector<BB *> SCC(const Loop *curLoop) const;

public:
    LivenessAnalysis(Function *graph) : graph_(graph), loopTree_(graph) {
        assert(graph_);
        graph_->buildDominatorTree();

        loopTree_.BuildLoopTree();
    }

    std::vector<BB *> CreateLinearOrder() const;

    void run();
};

} // namespace IRGen

#endif