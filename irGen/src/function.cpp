#include "function.hpp"
#include "instrDef.hpp"
#include <algorithm>
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

    newBB->SetFunction(this);

    anchor->AddPred(newBB);
    newBB->AddSucc(anchor, cond);
}

void Function::AddBBAsSucc(BB *newBB, BB *anchor, bool cond) {
    if (!anchor || !newBB)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    if ((newBB->GetFunction() && newBB->GetFunction() != this) || anchor->GetFunction() != this)
        throw std::runtime_error("BB in this graph in "s + CUR_FUNC_NAME);

    newBB->SetFunction(this);

    anchor->AddSucc(newBB, cond);
    newBB->AddPred(anchor);
}

void Function::DeletePreds(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    auto &p = bb->GetPreds();
    std::for_each(p.begin(), p.end(), [&bb](auto &b) { bb->DeletePred(b); });
}

void Function::DeleteSuccs(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    auto &s = bb->GetSuccs();
    std::for_each(s.begin(), s.end(), [&bb](auto &b) { bb->DeleteSucc(b); });
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

} // namespace IRGen
