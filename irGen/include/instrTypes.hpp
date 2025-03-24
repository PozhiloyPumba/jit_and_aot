#ifndef __INSTRTYPES_HPP__
#define __INSTRTYPES_HPP__

#include "helpers/genEnum.hpp"
#include <cstdint>
#include <string>
#include <type_traits>
#include <variant>

namespace IRGen {

// clang-format off
#define ENUM_TYPE_INSTR(c)  \
    c(I8)                   \
    c(I16)                  \
    c(I32)                  \
    c(I64)                  \
    c(U8)                   \
    c(U16)                  \
    c(U32)                  \
    c(U64)                  \
    c(VOID)                 \
    c(ERROR)
// clang-format on

CREATE_ENUM(InstrType, ENUM_TYPE_INSTR);

using ImmType = std::variant<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>;

constexpr static inline ImmType CreateImm(InstrType type, std::integral auto imm) {
    switch (type) {
    case InstrType::I8:
        return static_cast<int8_t>(imm);
    case InstrType::I16:
        return static_cast<int16_t>(imm);
    case InstrType::I32:
        return static_cast<int32_t>(imm);
    case InstrType::I64:
        return static_cast<int64_t>(imm);
    case InstrType::U8:
        return static_cast<uint8_t>(imm);
    case InstrType::U16:
        return static_cast<uint16_t>(imm);
    case InstrType::U32:
        return static_cast<uint32_t>(imm);
    case InstrType::U64:
        return static_cast<uint64_t>(imm);
    default:;
    }
    return ImmType();
}

constexpr static inline InstrType GetEnumFromVariant(ImmType imm) {
	return InstrType(imm.index());
}

constexpr static inline std::string toString(InstrType type, ImmType imm) {
    switch (type) {
    case InstrType::I8:
        return std::to_string(std::get<int8_t>(imm));
    case InstrType::I16:
        return std::to_string(std::get<int16_t>(imm));
    case InstrType::I32:
        return std::to_string(std::get<int32_t>(imm));
    case InstrType::I64:
        return std::to_string(std::get<int64_t>(imm));
    case InstrType::U8:
        return std::to_string(std::get<uint8_t>(imm));
    case InstrType::U16:
        return std::to_string(std::get<uint16_t>(imm));
    case InstrType::U32:
        return std::to_string(std::get<uint32_t>(imm));
    case InstrType::U64:
        return std::to_string(std::get<uint64_t>(imm));
    default:;
    }
    return "Something went wrong oops....";
}

} // namespace IRGen

#endif