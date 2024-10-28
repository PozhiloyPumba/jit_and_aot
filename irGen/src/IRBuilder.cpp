#include "IRBuilder.hpp"
#include "function.hpp"
#include "BB.hpp"
#include <cassert>

namespace IRGen {

Function *IRGenerator::CreateFunction(const std::string &name, InstrType retType, const std::vector<ParameterInstr *> &params) {
    auto *f = new Function();
    graph_.push_back(f);
    f->SetName(name);
    f->SetRetType(retType);
    f->SetParams(params);
    return f;
}
BB *IRGenerator::CreateEmptyBB(Function *parent) {
    auto *bb = new BB();
    BBs_.push_back(bb);
    if (parent != nullptr) {
        parent->AddBB(bb);
    }
    return bb;
}

void IRGenerator::Clear() {
    for (auto *f: graph_) {
        delete f;
    }
    graph_.clear();
    for (auto *bb : BBs_) {
        delete bb;
    }
    BBs_.clear();
}

}