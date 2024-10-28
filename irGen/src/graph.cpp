#include <stdexcept>
#include <string>
#include <algorithm>
#include <iostream>
#include "graph.hpp"

namespace IRGen {
using namespace std::literals;

void Graph::AddBB(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    bb->SetDefaultId();
    BBs_.push_back(bb);
    bb->SetGraph(this);
}

void Graph::AssasinateBB(BB *bb) {
	if (!bb)
		throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

	auto it = std::find(BBs_.begin(), BBs_.end(), bb);
    if (bb->GetGraph() != this || it == BBs_.end())
		throw std::runtime_error("BB not in this graph in "s + CUR_FUNC_NAME);

	BBs_.erase(it);
    bb->SetId(BB::INVALID_BB_ID);
    if (bb == lastBB_) {
        SetLastBB(nullptr);
    }
    DeletePreds(bb);
    DeleteSuccs(bb);
}

void Graph::AddBBAsPred(BB *newBB, BB *anchor) {
	if (!anchor || !newBB)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    if ((newBB->GetGraph() && newBB->GetGraph() != this) || anchor->GetGraph() != this)
        throw std::runtime_error("BB in this graph in "s + CUR_FUNC_NAME);

    newBB->SetGraph(this);

    anchor->AddPred(newBB);
	newBB->AddSucc(anchor);
}

void Graph::AddBBAsSucc(BB *newBB, BB *anchor) {
	if (!anchor || !newBB)
        throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

    if ((newBB->GetGraph() && newBB->GetGraph() != this) || anchor->GetGraph() != this)
        throw std::runtime_error("BB in this graph in "s + CUR_FUNC_NAME);

    newBB->SetGraph(this);

    anchor->AddSucc(newBB);
	newBB->AddPred(anchor);
}

void Graph::DeletePreds(BB *bb) {
    if (!bb)
		throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

	auto &p = bb->GetPreds();
	std::for_each(p.begin(), p.end(), [&bb](auto &b){bb->DeletePred(b);});
}

void Graph::DeleteSuccs(BB *bb) {
	if (!bb)
		throw std::runtime_error("Null BB in "s + CUR_FUNC_NAME);

	auto &s = bb->GetSuccs();
	std::for_each(s.begin(), s.end(), [&bb](auto &b){bb->DeleteSucc(b);});
}

void Graph::dump() {
	std::cout << IRGen::toString(retType_) << " " << name_ << "(";
	std::for_each(params_.begin(), params_.end(), [](const auto &p){
		std::cout << IRGen::toString(p.first) << " " << IRGen::typeToReg(p.first) << p.second << " ";
	});
	std::cout << ")" << std::endl;
	std::for_each(BBs_.begin(), BBs_.end(), [](auto *b){b->dump();});
}

}
