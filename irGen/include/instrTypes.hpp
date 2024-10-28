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
    c(U64) \
    c(VOID) \
    c(ERROR)

CREATE_ENUM(InstrType, ENUM_TYPE_INSTR);

[[maybe_unused]]static std::string typeToReg(InstrType type) {
	switch(type){
		case InstrType::U64: return "v";
		case InstrType::U32: return "a";
		default:;
	}
	return "h";
}

namespace {

template<size_t SZ>
[[maybe_unused]] InstrType GetInstrTypeUnsigned() { return InstrType::ERROR; }

template<> InstrType GetInstrTypeUnsigned<1>() { return InstrType::U8; }
template<> InstrType GetInstrTypeUnsigned<2>() { return InstrType::U16; }
template<> InstrType GetInstrTypeUnsigned<4>() { return InstrType::U32; }
template<> InstrType GetInstrTypeUnsigned<8>() { return InstrType::U64; }

template<size_t SZ>
[[maybe_unused]] InstrType GetInstrTypeSigned() { return InstrType::ERROR; }

template<> InstrType GetInstrTypeSigned<1>() { return InstrType::I8; }
template<> InstrType GetInstrTypeSigned<2>() { return InstrType::I16; }
template<> InstrType GetInstrTypeSigned<4>() { return InstrType::I32; }
template<> InstrType GetInstrTypeSigned<8>() { return InstrType::I64; }

template<typename T>
[[maybe_unused]] InstrType GetInstrType() {
    static_assert(std::is_integral_v<T> == true);
    if constexpr (std::is_unsigned_v<T>)
        return GetInstrTypeUnsigned<sizeof(T)>();
    else return GetInstrTypeSigned<sizeof(T)>();
}

}

}

#endif