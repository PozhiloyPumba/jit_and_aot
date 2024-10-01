#ifndef __INSTRTYPES_HPP__
#define __INSTRTYPES_HPP__

#include "helpers/genEnum.hpp"

namespace IRGen {

#define ENUM_TYPE_INSTR(c) \
    c(I8) \
    c(I16) \
    c(I32) \
    c(I64) \
    c(U8) \
    c(U16) \
    c(U32) \
    c(U64)

CREATE_ENUM(InstrType, ENUM_TYPE_INSTR);

[[maybe_unused]]static std::string typeToReg(InstrType type) {
	switch(type){
		case InstrType::U64: return "v";
		case InstrType::U32: return "a";
		default:;
	}
	return "h";
}

}

#endif