#include "passes/peepholePass.hpp"
#include "IRBuilder.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace IRGen {

void PeepHolePass::Run(Function *func) const {
    for (auto &bb : func->GetAllBBs()) {
        for (auto *cur = bb->GetBeginBB(); cur != nullptr;) {
            switch (cur->GetOpcode()) {
            case Opcode::SUB: {
                cur = SubPeephole(static_cast<TwoValInstr *>(cur));
                break;
            }
            case Opcode::SUBI: {
                cur = SubIPeephole(static_cast<ValAndImmInstr *>(cur));
                break;
            }
            case Opcode::SHR: {
                cur = ShrPeephole(static_cast<TwoValInstr *>(cur));
                break;
            }
            case Opcode::SHRI: {
                cur = ShrIPeephole(static_cast<ValAndImmInstr *>(cur));
                break;
            }
            case Opcode::AND: {
                cur = AndPeephole(static_cast<TwoValInstr *>(cur));
                break;
            }
            case Opcode::ANDI: {
                cur = AndIPeephole(static_cast<ValAndImmInstr *>(cur));
                break;
            }
            default: {
                cur = cur->GetNextInstr();
            }
            }
        }
    }
}

Instruction *PeepHolePass::SubPeephole(TwoValInstr *instr) const {
    if (!instr || instr->GetOpcode() != Opcode::SUB)
        throw std::runtime_error("Wrong instr with opcode "s + IRGen::toString(instr->GetOpcode()) + " " + CUR_FUNC_NAME);

    const auto &inputs = instr->GetInputs();
    auto &builder = InstructionBuilder::GetInstance();

    if (inputs[0] == inputs[1]) { // sub x, x -> movi 0
        auto *subs = builder.BuildMovI(IRGen::CreateImm(instr->GetType(), 0));
        auto *bb = instr->GetBB();
        bb->ReplaceInstr(instr, {subs});
        for (auto &user : instr->GetUsers()) {
            auto &inputs = user->GetInputs();
            std::replace(inputs.begin(), inputs.end(), static_cast<Instruction *>(instr), static_cast<Instruction *>(subs));
        }
        return subs;
    }
    return instr->GetNextInstr();
}

Instruction *PeepHolePass::ShrPeephole(TwoValInstr *instr) const {
    if (!instr || instr->GetOpcode() != Opcode::SHR)
        throw std::runtime_error("Wrong instr with opcode "s + IRGen::toString(instr->GetOpcode()) + " " + CUR_FUNC_NAME);

    return instr->GetNextInstr();
}

Instruction *PeepHolePass::AndPeephole(TwoValInstr *instr) const {
    if (!instr || instr->GetOpcode() != Opcode::AND)
        throw std::runtime_error("Wrong instr with opcode "s + IRGen::toString(instr->GetOpcode()) + " " + CUR_FUNC_NAME);

    const auto &inputs = instr->GetInputs();
    auto &builder = InstructionBuilder::GetInstance();

    if (inputs[0] == inputs[1]) { // and x, x -> pass x to others
        auto *bb = instr->GetBB();
        auto *target = inputs[0];
        auto *next = instr->GetNextInstr();
        for (auto &user : instr->GetUsers()) {
            auto &inputs = user->GetInputs();
            std::replace(inputs.begin(), inputs.end(), static_cast<Instruction *>(instr), static_cast<Instruction *>(target));
        }
        bb->AssasinateInstr(instr);
        return next;
    }
    return instr->GetNextInstr();
}

Instruction *PeepHolePass::SubIPeephole(ValAndImmInstr *instr) const {
    if (!instr || instr->GetOpcode() != Opcode::SUBI)
        throw std::runtime_error("Wrong instr with opcode "s + IRGen::toString(instr->GetOpcode()) + " " + CUR_FUNC_NAME);

    Instruction *next = instr->GetNextInstr();
    std::visit(
        [&instr, &next](auto &imm) {
            using T = std::decay_t<decltype(imm)>;

            auto &builder = InstructionBuilder::GetInstance();
            const auto &inputs = instr->GetInputs();
            if (imm == static_cast<T>(0)) { // sub x, 0 -> pass x to others
                auto *bb = instr->GetBB();
                auto *target = inputs[0];
                for (auto &user : instr->GetUsers()) {
                    auto &inputs = user->GetInputs();
                    std::replace(inputs.begin(), inputs.end(), static_cast<Instruction *>(instr),
                                 static_cast<Instruction *>(target));
                }
                next = instr->GetNextInstr();
                bb->AssasinateInstr(instr);
                return;
            }
            if (imm == static_cast<T>(1)) { // sub x, 1 -> dec x
                auto *target = inputs[0];
                auto *subs = builder.BuildDec(instr->GetType(), target);
                auto *bb = instr->GetBB();
                bb->ReplaceInstr(instr, {subs});
                for (auto &user : instr->GetUsers()) {
                    auto &inputs = user->GetInputs();
                    std::replace(inputs.begin(), inputs.end(), static_cast<Instruction *>(instr),
                                 static_cast<Instruction *>(subs));
                }
                next = subs;
            }
        },
        instr->GetImm());

    return next;
}

Instruction *PeepHolePass::ShrIPeephole(ValAndImmInstr *instr) const {
    if (!instr || instr->GetOpcode() != Opcode::SHRI)
        throw std::runtime_error("Wrong instr with opcode "s + IRGen::toString(instr->GetOpcode()) + " " + CUR_FUNC_NAME);

    Instruction *next = instr->GetNextInstr();
    std::visit(
        [&instr, &next](auto &imm) {
            using T = std::decay_t<decltype(imm)>;

            auto &builder = InstructionBuilder::GetInstance();
            const auto &inputs = instr->GetInputs();

            if (imm == static_cast<T>(0)) { // shr x, 0 -> pass x to others
                auto *bb = instr->GetBB();
                auto *target = inputs[0];
                for (auto &user : instr->GetUsers()) {
                    auto &inputs = user->GetInputs();
                    std::replace(inputs.begin(), inputs.end(), static_cast<Instruction *>(instr),
                                 static_cast<Instruction *>(target));
                }
                next = instr->GetNextInstr();
                bb->AssasinateInstr(instr);
                return;
            }

            if (imm >= sizeof(T) * 8) { // shr x, BIG_NUMBER, mov 0
                auto *subs = builder.BuildMovI(IRGen::CreateImm(instr->GetType(), 0));
                auto *bb = instr->GetBB();
                next = instr->GetNextInstr();
                bb->ReplaceInstr(instr, {subs});
                for (auto &user : instr->GetUsers()) {
                    auto &inputs = user->GetInputs();
                    std::replace(inputs.begin(), inputs.end(), static_cast<Instruction *>(instr),
                                 static_cast<Instruction *>(subs));
                }
                return;
            }

            auto *tmp = instr->GetNextInstr();
            if (tmp && (tmp->GetOpcode() == Opcode::SHLI) && (tmp->GetInputs()[0] == instr) &&
                std::holds_alternative<T>(static_cast<ValAndImmInstr *>(tmp)->GetImm())) {

                // shl((shr x, n), n) -> and x, (2^(size) - 1) - (2^(n) - 1)
                auto *subs = builder.BuildAndI(
                    inputs[0],
                    IRGen::CreateImm(instr->GetType(), std::numeric_limits<T>::max() - ((static_cast<T>(1) << imm) - 1)));
                auto *bb = instr->GetBB();
                next = instr->GetNextInstr();
                bb->ReplaceInstr(instr, {subs});
                for (auto &user : instr->GetUsers()) {
                    auto &inputs = user->GetInputs();
                    std::replace(inputs.begin(), inputs.end(), static_cast<Instruction *>(instr),
                                 static_cast<Instruction *>(subs));
                }
                bb->AssasinateInstr(tmp);
                return;
            }
        },
        instr->GetImm());

    return next;
}

Instruction *PeepHolePass::AndIPeephole(ValAndImmInstr *instr) const {
    if (!instr || instr->GetOpcode() != Opcode::ANDI)
        throw std::runtime_error("Wrong instr with opcode "s + IRGen::toString(instr->GetOpcode()) + " " + CUR_FUNC_NAME);

    Instruction *next = instr->GetNextInstr();
    std::visit(
        [&instr, &next](auto &imm) {
            using T = std::decay_t<decltype(imm)>;

            auto &builder = InstructionBuilder::GetInstance();
            const auto &inputs = instr->GetInputs();

            if (imm == static_cast<T>(0)) { // and x, 0 -> movi 0
                auto *subs = builder.BuildMovI(IRGen::CreateImm(instr->GetType(), 0));
                auto *bb = instr->GetBB();
                next = instr->GetNextInstr();
                bb->ReplaceInstr(instr, {subs});
                for (auto &user : instr->GetUsers()) {
                    auto &inputs = user->GetInputs();
                    std::replace(inputs.begin(), inputs.end(), static_cast<Instruction *>(instr),
                                 static_cast<Instruction *>(subs));
                }
                return;
            }

            if (imm == std::numeric_limits<T>::max()) { // and x, oxFFFF..., pass x to other
                auto *bb = instr->GetBB();
                auto *target = inputs[0];
                for (auto &user : instr->GetUsers()) {
                    auto &inputs = user->GetInputs();
                    std::replace(inputs.begin(), inputs.end(), static_cast<Instruction *>(instr),
                                 static_cast<Instruction *>(target));
                }
                next = instr->GetNextInstr();
                bb->AssasinateInstr(instr);
                return;
            }
        },
        instr->GetImm());

    return next;
}

} // namespace IRGen