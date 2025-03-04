#ifndef __OPCODES_HPP__
#define __OPCODES_HPP__

#include "helpers/genEnum.hpp"

namespace IRGen {

// clang-format off
#define ENUM_OPCODES(c) \
    c(MUL)              \
    c(ADD)              \
    c(SUB)              \
    c(SHR)              \
    c(AND)              \
    c(ADDI)             \
    c(SUBI)             \
    c(SHRI)             \
    c(SHLI)             \
    c(ANDI)             \
    c(MOVI)             \
    c(CMP)              \
    c(JA)               \
    c(JMP)              \
    c(RET)              \
    c(DEC)              \
    c(CAST)             \
    c(PHI)              \
    c(PARAM)            \
    c(INVALID_INSTR)
// clang-format on

CREATE_ENUM(Opcode, ENUM_OPCODES);

} // namespace IRGen

#endif