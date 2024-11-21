#include "function.hpp"
#include "instrDef.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

namespace IRGen {
using namespace std::literals;

void Function::AddBB(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    BBs_.push_back(bb);
    bb->SetFunction(this);
}

void Function::AssasinateBB(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    auto it = std::find(BBs_.begin(), BBs_.end(), bb);
    if (bb->GetFunction() != this || it == BBs_.end())
        throw std::runtime_error("BB not in this graph in "s + CUR_FUNC_NAME);

    BBs_.erase(it);
    bb->SetId(BB::INVALID_BB_ID);
    if (bb == lastBB_) {
        SetLastBB(nullptr);
    }
    DeletePreds(bb);
    DeleteSuccs(bb);
}

void Function::AddBBAsPred(BB *newBB, BB *anchor, bool cond) {
    if (!anchor || !newBB)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    if ((newBB->GetFunction() && newBB->GetFunction() != this) || anchor->GetFunction() != this)
        throw std::runtime_error("BB in this graph in "s + CUR_FUNC_NAME);

    anchor->AddPred(newBB);
    newBB->AddSucc(anchor, cond);
}

void Function::AddBBAsSucc(BB *newBB, BB *anchor, bool cond) {
    if (!anchor || !newBB)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    if ((newBB->GetFunction() && newBB->GetFunction() != this) || anchor->GetFunction() != this)
        throw std::runtime_error("BB not in this func in "s + CUR_FUNC_NAME);

    anchor->AddSucc(newBB, cond);
    newBB->AddPred(anchor);
}

void Function::DeletePreds(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    auto &p = bb->GetPreds();
    std::for_each(p.begin(), p.end(), [&bb](auto &b) {
        bb->DeletePred(b);
        b->DeleteSucc(bb);
    });
}

void Function::DeleteSuccs(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    auto &s = bb->GetSuccs();
    std::for_each(s.begin(), s.end(), [&bb](auto &b) {
        bb->DeleteSucc(b);
        b->DeletePred(bb);
    });
}

bool Function::DominatedBy(BB *child, BB *parent) const {
    assert(child);
    assert(parent);
    assert(dTreeBuild);
    if (child->GetFunction() != this || parent->GetFunction() != this)
        throw std::runtime_error("not known BB in "s + CUR_FUNC_NAME);

    auto *tmp = domTreeParents_.at(child);

    while (tmp) {
        if (tmp == parent)
            return true;
        tmp = domTreeParents_.at(tmp);
    }
    return false;
}

BB *Function::GetIDomFor(BB *bb) const {
    assert(bb);
    assert(dTreeBuild);
    if (bb->GetFunction() != this)
        throw std::runtime_error("not known BB in "s + CUR_FUNC_NAME);
    return domTreeParents_.at(bb);
}

void Function::dump() const {
    std::cout << IRGen::toString(retType_) << " " << name_ << "(";
    const char *padding = "";
    std::for_each(params_.begin(), params_.end(), [&padding](const auto *p) {
        std::cout << padding << p->toString();
        padding = ", ";
    });
    std::cout << ")" << std::endl;
    std::for_each(BBs_.begin(), BBs_.end(), [](auto *b) { b->dump(); });
}

std::unordered_map<BB *, std::unordered_set<BB *>> &Function::buildDominatorTree() {
    if (dTreeBuild)
        return domTree_;
    std::unordered_set<uint64_t> visited;
    std::function<void(const BB *, const BB *)> dfs;

    dfs = [&dfs, &visited](const BB *bb, const BB *checked_bb) {
        if (!bb || bb == checked_bb)
            return;

        visited.insert(bb->GetId());
        for (const auto *s : bb->GetSuccs()) {
            if (!s)
                continue;
            if (visited.find(s->GetId()) == visited.end())
                dfs(s, checked_bb);
        }
    };

    for (auto *cur_bb : BBs_) {
        assert(cur_bb != nullptr);
        visited.clear();
        dfs(firstBB_, cur_bb);
        std::copy_if(BBs_.begin(), BBs_.end(), std::inserter(domTree_[cur_bb], domTree_[cur_bb].end()),
                     [cur_bb, &visited](const auto *bb) { return (bb != cur_bb) && visited.find(bb->GetId()) == visited.end(); });
    }

    for (auto &[bb, dominated] : domTree_) {
        std::unordered_set<BB *> saved = dominated;
        for (const auto &dominated_bb : saved) {
            std::for_each(domTree_[dominated_bb].begin(), domTree_[dominated_bb].end(),
                          [&dominated](auto *bb) { dominated.erase(bb); });
        }
    }
    domTreeParents_[firstBB_] = nullptr;
    for (auto &[bb, dominated] : domTree_) {
        std::for_each(dominated.begin(), dominated.end(), [this, &bb](auto *cur) { domTreeParents_[cur] = bb; });
    }
    dTreeBuild = true;
    return domTree_;
}

} // namespace IRGen
