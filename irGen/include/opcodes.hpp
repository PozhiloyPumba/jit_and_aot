#ifndef __OPCODES_HPP__
#define __OPCODES_HPP__

#include "helpers/genEnum.hpp"

namespace IRGen {

#define ENUM_OPCODES(c) \
    c(MUL) \
    c(ADDI) \
    c(MOVI) \
    c(CMP) \
    c(JA) \
    c(JMP) \
    c(RET) \
	c(CAST) \
	c(PHI) \
	c(PARAM) \
    c(INVALID_INSTR)

CREATE_ENUM(Opcode, ENUM_OPCODES)

}

#endif