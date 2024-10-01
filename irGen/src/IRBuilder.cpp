#include "IRBuilder.hpp"
#include "graph.hpp"
#include "BB.hpp"

namespace IRGen {

void IRGenerator::CreateGraph() {
	if (graph_ == nullptr) {
		graph_ = new Graph();
	}
}
BB *IRGenerator::CreateEmptyBB() {
    auto *bb = new BB();
    BBs_.push_back(bb);
    if (graph_ != nullptr) {
        graph_->AddBB(bb);
    }
    return bb;
}

void IRGenerator::Clear() {
    for (auto *bb : BBs_) {
        delete bb;
    }
    BBs_.clear();

    delete graph_;
    graph_ = nullptr;
}

}