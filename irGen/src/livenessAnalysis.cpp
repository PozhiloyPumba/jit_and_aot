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

void LivenessAnalysis::run() {
    auto order = CreateLinearOrder();

    // set linear and live numbers

    uint64_t liveNumber = 0;
    uint64_t linearNumber = 0;
    for (auto bb : order) {
        bb->GetLiveRange().start = liveNumber;
        for (auto curIt = bb->GetBeginBB(), endIt = bb->GetEndBB()->GetNextInstr(); curIt != endIt;
             curIt = curIt->GetNextInstr()) {
            curIt->GetLinearNumber() = linearNumber++;
            liveNumber = curIt->IsPhiInstr() ? liveNumber : liveNumber + 2;
            curIt->GetLiveNumber() = liveNumber;
        }
        liveNumber += 2;
        bb->GetLiveRange().end = liveNumber;
    }

    std::unordered_map<BB *, std::unordered_set<Instruction *>> bbLiveSets;
    using namespace std::ranges;
    for (auto *bb : order | views::reverse) {
        auto &liveIn = bbLiveSets[bb];
        LiveRange &bbLiveRange = bb->GetLiveRange();

        for (auto *succ : bb->GetSuccs()) {
            if (!succ)
                continue;

            auto succIt = bbLiveSets.find(succ);
            if (succIt != bbLiveSets.end()) {
                auto succLiveset = succIt->second;
                std::copy(succLiveset.begin(), succLiveset.end(), std::inserter(liveIn, liveIn.end()));
            }
            auto *curInstr = succ->GetBeginBB();
            while (curInstr && curInstr->IsPhiInstr()) {
                liveIn.insert(curInstr);
                curInstr = curInstr->GetNextInstr();
            }
        }

        for (auto *op : liveIn) {
            op->GetLiveRange() += bb->GetLiveRange();
        }

        for (auto curIt = bb->GetEndBB(), endIt = bb->GetBeginBB()->GetPrevInstr(); curIt != endIt;
             curIt = curIt->GetPrevInstr()) {
            if (curIt->IsPhiInstr())
                break;

            auto &instrLI = curIt->GetLiveRange();
            instrLI += LiveRange(curIt->GetLiveNumber(), curIt->GetLiveNumber() + 2);
            liveIn.erase(curIt);

            for (auto *inp : curIt->GetInputs()) {
                inp->GetLiveRange() += LiveRange(bbLiveRange.start, curIt->GetLiveNumber());
                liveIn.insert(inp);
            }
        }

        auto *curInstr = bb->GetBeginBB();
        while (curInstr && curInstr->IsPhiInstr()) {
            liveIn.erase(curInstr);
            curInstr = curInstr->GetNextInstr();
        }

        if (bb->IsLoopHeader()) {
            LiveRange loopLR = bb->GetLiveRange();

            for (auto &latch : bb->GetLoop()->GetLatches())
                loopLR.end = std::max(loopLR.end, latch->GetLiveRange().end);

            for (auto &instr : liveIn)
                instr->GetLiveRange() += loopLR;
        }
    }
}

} // namespace IRGen
