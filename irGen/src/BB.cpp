#include "BB.hpp"
#include <algorithm>
#include <function.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

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

void BB::AddSucc(BB *bb, bool cond) {
    if (!bb)
        throw std::runtime_error("Null BB passed to "s + CUR_FUNC_NAME);

    succs_[cond] = bb;
}

void BB::DeleteSucc(BB *bb) {
    if (!bb)
        throw std::runtime_error("Null BB passed to "s + CUR_FUNC_NAME);

    auto it = std::find(succs_.begin(), succs_.end(), bb);
    if (it != succs_.end())
        *it = nullptr;
    else
        throw std::runtime_error("No find successor in "s + CUR_FUNC_NAME);
}

void BB::InsertInstrBefore(Instruction *insert, Instruction *anchor) {
    if (!insert)
        throw std::runtime_error("Null insert instr in "s + CUR_FUNC_NAME);
    if (!anchor)
        throw std::runtime_error("Null anchor instr in "s + CUR_FUNC_NAME);
    if (anchor->GetBB() != this)
        throw std::runtime_error("Anchor BB diff from cur BB in "s + CUR_FUNC_NAME);

    insert->SetBB(this);
    auto prev = anchor->GetPrevInstr();
    insert->SetPrevInstr(prev);
    insert->SetNextInstr(anchor);
    anchor->SetPrevInstr(insert);
    if (!prev)
        beginBB_ = insert;
    else
        prev->SetNextInstr(insert);
}

void BB::InsertInstrAfter(Instruction *insert, Instruction *anchor) {
    if (!insert)
        throw std::runtime_error("Null insert instr in "s + CUR_FUNC_NAME);
    if (!anchor)
        throw std::runtime_error("Null anchor instr in "s + CUR_FUNC_NAME);
    if (anchor->GetBB() != this)
        throw std::runtime_error("Anchor BB diff from cur BB in "s + CUR_FUNC_NAME);

    insert->SetBB(this);
    auto next = anchor->GetNextInstr();
    insert->SetNextInstr(next);
    insert->SetPrevInstr(anchor);
    anchor->SetNextInstr(insert);
    if (!next)
        endBB_ = insert;
    else
        next->SetPrevInstr(insert);
}

void BB::AssasinateInstr(Instruction *instr) {
    if (!instr || instr->GetBB() != this)
        throw std::runtime_error("Wrong instr "s + CUR_FUNC_NAME);

	for(auto &input: instr->GetInputs()) {
		input->RemoveUser(instr);
	}

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

void BB::ReplaceInstr(Instruction *target, const std::vector<Instruction *> &replace) {
    auto *cur = target;
    for (auto &repl : replace) {
        InsertInstrAfter(repl, cur);
        cur = repl;
    }
    AssasinateInstr(target);
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
bool BB::DominatedBy(BB *other) {
    return graph_->DominatedBy(this, other);
}

BB *BB::GetIDom() {
    return graph_->GetIDomFor(this);
}

bool BB::IsLoopHeader() const {
    if (!loop_)
        throw std::runtime_error("Null loop for bb in "s + CUR_FUNC_NAME);
    return loop_->GetHeader() == this;
}

void BB::dump() const {
    auto *cur = beginBB_;
    if (!cur) {
        std::cout << "Empty" << std::endl;
        return;
    }
    std::cout << "%" << bbId_ << ":" << std::endl;
    while (cur) {
        std::cout << "\t" << cur->toString() << std::endl;
        cur = cur->GetNextInstr();
    }
    std::cout << std::endl;
}

} // namespace IRGen
