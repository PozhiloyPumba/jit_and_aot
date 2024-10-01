#include <string>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "BB.hpp"

namespace IRGen {
using namespace std::literals;

void BB::AddPred(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB passed to "s + CUR_FUNC_NAME);

    preds_.push_back(bb);
}

void BB::DeletePred(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB passed to "s + CUR_FUNC_NAME);

    auto it = std::find(preds_.begin(), preds_.end(), bb);
    if (it != preds_.end())
        preds_.erase(it);
    else
        throw std::runtime_error("No find predecessor in "s + CUR_FUNC_NAME); 
}

void BB::AddSucc(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB passed to "s + CUR_FUNC_NAME);

    if (succs_.size() > 1)
        throw std::runtime_error("Attempt to add more than 2 successor "s + CUR_FUNC_NAME);

    succs_.push_back(bb);
}

void BB::DeleteSucc(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB passed to "s + CUR_FUNC_NAME);

    auto it = std::find(succs_.begin(), succs_.end(), bb);
    if (it != succs_.end())
        succs_.erase(it);
    else
        throw std::runtime_error("No find successor in "s + CUR_FUNC_NAME); 
}

void BB::InsertInstrBefore(Instruction *insert, Instruction *anchor) {
	if(!insert)
        throw std::runtime_error("Null insert instr in "s + CUR_FUNC_NAME);
	if(!anchor)
        throw std::runtime_error("Null anchor instr in "s + CUR_FUNC_NAME);
	if(anchor->GetBB() != this)
        throw std::runtime_error("Anchor BB diff from cur BB in "s + CUR_FUNC_NAME);

	insert->SetBB(this);
	auto prev = anchor->GetPrevInstr();
	insert->SetPrevInstr(prev);
	insert->SetNextInstr(anchor);
	prev->SetNextInstr(insert);
	anchor->SetPrevInstr(insert);
	if(!prev)
		beginBB_ = insert;
}

void BB::InsertInstrAfter(Instruction *insert, Instruction *anchor) {
	if(!insert)
        throw std::runtime_error("Null insert instr in "s + CUR_FUNC_NAME);
	if(!anchor)
        throw std::runtime_error("Null anchor instr in "s + CUR_FUNC_NAME);
	if(anchor->GetBB() != this)
        throw std::runtime_error("Anchor BB diff from cur BB in "s + CUR_FUNC_NAME);
	
	insert->SetBB(this);
	auto next = anchor->GetNextInstr();
	insert->SetNextInstr(next);
	insert->SetPrevInstr(anchor);
	next->SetPrevInstr(insert);
	anchor->SetNextInstr(insert);
	if(!next)
		endBB_ = insert;
}

void BB::AssasinateInstr(Instruction *instr) {
    if (!instr || instr->GetBB() != this)
        throw std::runtime_error("Wrong instr "s + CUR_FUNC_NAME);

    instr->SetBB(nullptr); // unlink
    auto prev = instr->GetPrevInstr();
    auto next = instr->GetNextInstr();
    instr->SetPrevInstr(nullptr);
    instr->SetNextInstr(nullptr);

    if (prev)
        prev->SetNextInstr(next);
    else
        beginBB_ = next;

    if (next)
        next->SetPrevInstr(prev);
    else
        endBB_ = prev;
}

void BB::AddInstrForward(Instruction *instr) {
	if (!instr || instr->GetBB() || instr->GetPrevInstr() || instr->GetNextInstr())
        throw std::runtime_error("Wrong instr "s + CUR_FUNC_NAME);

    instr->SetBB(this);

    if (!beginBB_) {
        beginBB_ = instr;
        endBB_ = instr;
    } else {
        instr->SetNextInstr(beginBB_);
        beginBB_->SetPrevInstr(instr);
        beginBB_ = instr;
    }
}

void BB::AddInstrBackward(Instruction *instr) {
	if (!instr || instr->GetBB() || instr->GetPrevInstr() || instr->GetNextInstr())
        throw std::runtime_error("Wrong instr "s + CUR_FUNC_NAME);

    instr->SetBB(this);

    if (!beginBB_) {
        beginBB_ = instr;
        endBB_ = instr;
    } else {
        instr->SetPrevInstr(endBB_);
        endBB_->SetNextInstr(instr);
        endBB_ = instr;
    }
}

void BB::dump() {
	auto *cur = beginBB_;
	if (!cur) {
		std::cout << "Empty" << std::endl;
		return;
	}
	std::cout << bbId_ << ":" << std::endl;
	while(cur) {
		std::cout << "\t" << cur->toString() << std::endl;
		cur = cur->GetNextInstr();
	}
	std::cout << std::endl;
}

}
