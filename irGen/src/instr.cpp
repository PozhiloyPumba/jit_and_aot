#include "instr.hpp"
#include "BB.hpp"
#include "helpers/defines.hpp"
#include <stdexcept>
#include <string>

namespace IRGen {
using namespace std::literals;

void Instruction::Euthanasia() {
    if (!parentBB_)
        throw std::runtime_error("Null BB in removing from BB"s + CUR_FUNC_NAME);

    parentBB_->AssasinateInstr(this);
}

void Instruction::InsertBefore(Instruction *toInsert) {
    if (!parentBB_)
        throw std::runtime_error("Null BB in anchor instr "s + CUR_FUNC_NAME);

    parentBB_->InsertInstrBefore(toInsert, this);
}

void Instruction::InsertAfter(Instruction *toInsert) {
    if (!parentBB_)
        throw std::runtime_error("Null BB in anchor instr "s + CUR_FUNC_NAME);

    parentBB_->InsertInstrAfter(toInsert, this);
}

bool Instruction::DominatedBy(Instruction *other) {
    if (!other)
        throw std::runtime_error("Null instr in "s + CUR_FUNC_NAME);

    if (parentBB_ == other->GetBB()) {
        auto *curInstr = parentBB_->GetBeginBB();
        while (curInstr != this) {
            if (curInstr == other)
                return true;
            curInstr = curInstr->next_;
        }
        return false;
    } else {
        return parentBB_->DominatedBy(other->GetBB());
    }
}

} // namespace IRGen