#include "livenessAnalysis.hpp"
#include <functional>
#include <iostream>
#include <ranges>
#include <unordered_set>

namespace IRGen {

void LivenessAnalysis::SCCUtil(BB *bb, BBmapT<int> &disc, BBmapT<int> &low, std::stack<BB *> &st, BBmapT<bool> &stackMember,
                               std::vector<BB *> &res) const {
    static int time = 0;

    disc[bb] = low[bb] = ++time;
    st.push(bb);
    stackMember[bb] = true;

    const auto &succs = bb->GetSuccs();
    for (auto curIt = succs.begin(), endIt = succs.end(); curIt != endIt; ++curIt) {
        if (*curIt == nullptr)
            continue;
        BB *curBB = *curIt;

        if (!disc[curBB]) {
            SCCUtil(curBB, disc, low, st, stackMember, res);
            low[bb] = std::min(low[bb], low[curBB]);
        } else if (stackMember[curBB] == true)
            low[bb] = std::min(low[bb], disc[curBB]);
    }

    if (low[bb] == disc[bb]) {
        while (st.top() != bb) {
            auto tmp = st.top();
            stackMember[tmp] = false;
            st.pop();
        }
        BB *w = st.top();
        stackMember[w] = false;
        st.pop();
        res.push_back(w);
    }
}

std::vector<BB *> LivenessAnalysis::SCC(const Loop *curLoop) const {
    BBmapT<int> disc;
    BBmapT<int> low;
    BBmapT<bool> stackMember;
    std::stack<BB *> st;

    std::vector<BB *> res;
    for (auto *bb : graph_->GetAllBBs()) {
        if (!disc[bb] && curLoop->ContainsInSubloops(bb)) {
            SCCUtil(bb, disc, low, st, stackMember, res);
        }
    }

    return res;
}

std::vector<BB *> LivenessAnalysis::CreateLinearOrder() const {
    std::unordered_set<BB *> markers_;

    auto *begin = graph_->GetFirstBB();

    std::vector<BB *> linOrder; // result

    bool findIrr = false;

    std::function<void(const Loop *)> addCycle;
    addCycle = [this, &linOrder, &addCycle, &findIrr](const Loop *l) {
        if (findIrr)
            return;
        std::unordered_map<BB *, bool> memLatches;
        for (auto &latch : l->GetLatches()) {
            auto &succs = latch->GetSuccs();
            auto predicate = (succs[0] == l->GetHeader()) ? false : true;
            memLatches[latch] = predicate;
            latch->DeleteSucc(l->GetHeader());
        }

        auto order = SCC(l);

        using namespace std::ranges;

        for (auto bb : order | views::reverse) {
            auto *curLoop = bb->GetLoop();
            if (curLoop == l) {
                linOrder.push_back(bb);
                continue;
            }

            if (!curLoop->GetReducible()) {
                findIrr = true;
                continue;
            }

            if (loopTree_.IsInnerLoopOf(curLoop, l))
                addCycle(curLoop);
        }

        for (auto &[latch, predicate] : memLatches) {
            latch->AddSucc(l->GetHeader(), predicate);
        }
    };

    addCycle(loopTree_.GetRootLoop());
    return findIrr ? std::vector<BB *>() : linOrder;
}

} // namespace IRGen
