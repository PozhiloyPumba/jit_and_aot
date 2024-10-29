#include "loopAnalyzer.hpp"
#include <algorithm>
#include <functional>
#include <iostream>

namespace IRGen {

LoopAnalyzer::~LoopAnalyzer() {
    for (auto l : loops_) {
        delete l.second;
    }
    delete root_;
}

void LoopAnalyzer::Dfs(BB *cur) {
    auto getLoop = [this](auto bb) {
        auto it = loops_.find(bb);
        return (it == loops_.end()) ? CreateLoop(bb) : it->second;
    };

    markers_[cur] = BLACK | GREY;
    for (auto s : cur->GetSuccs()) {
        if (!s)
            continue;
        if (markers_[s] & GREY) {
            Loop *l = getLoop(s);
            l->SetHeader(s);
            l->GetLatches().insert(cur);
            l->SetReducible(cur->DominatedBy(s));
        } else if (BLACK != (markers_[s] & BLACK))
            Dfs(s);
    }
    markers_[cur] &= ~GREY;
}

void LoopAnalyzer::BuildLoopTree() {
    graph_->buildDominatorTree();
    markers_.clear();
    Dfs(graph_->GetFirstBB());

    markers_.clear();
    std::vector<BB *> po;
    std::function<void(BB *)> poRun;
    poRun = [&poRun, this, &po](BB *bb) {
        markers_[bb] = BLACK;
        for (auto *s : bb->GetSuccs()) {
            if (s && !markers_[s]) {
                poRun(s);
            }
        }
        po.push_back(bb);
    };
    poRun(graph_->GetFirstBB());

    for (auto *loopHeader : po) {
        auto loopIt = loops_.find(loopHeader);
        if (loopIt == loops_.end())
            continue;

        auto *loop = loopIt->second;
        loopHeader->GetLoop() = loop;
        loop->GetBody().insert(loopHeader);
        if (loop->GetReducible()) {
            for (auto *l : loop->GetLatches()) {
                markers_.clear();
                markers_[loopHeader] = BLACK;
                AddSourcesFromLatch(l, loop);
            }
        } else {
            std::copy(loop->GetLatches().begin(), loop->GetLatches().end(),
                      std::inserter(loop->GetBody(), loop->GetBody().end()));
        }
    }
    root_ = new Loop();
    auto &allBBs = graph_->GetAllBBs();
    for (auto *bb : allBBs) {
        if (!bb->GetLoop()) {
            bb->GetLoop() = root_;
            root_->GetBody().insert(bb);
        }
    }
    for (auto l : loops_) {
        if (!l.second->GetOuterLoop()) {
            l.second->SetOuterLoop(root_);
            root_->AddInnerLoop(l.second);
        }
    }
}

void LoopAnalyzer::AddSourcesFromLatch(BB *latch, Loop *loop) {
    markers_[latch] = BLACK;
    do {
        auto &bbLoop = latch->GetLoop();
        if (!bbLoop) {
            bbLoop = loop;
            loop->GetBody().insert(latch);
            break;
        }
        if (bbLoop != loop) {
            loop->AddInnerLoop(bbLoop);
            bbLoop->SetOuterLoop(loop);
        }
    } while (0);

    for (auto *p : latch->GetPreds()) {
        if (!markers_[p])
            AddSourcesFromLatch(p, loop);
    }
}

} // namespace IRGen