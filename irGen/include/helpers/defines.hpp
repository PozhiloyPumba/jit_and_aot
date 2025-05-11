#ifndef __HELPERS_DEFINES_HPP__
#define __HELPERS_DEFINES_HPP__

#define NO_COPY(name)                                                                                                            \
    name(const name &) = delete;                                                                                                 \
    name &operator=(const name &) = delete;

#define NO_MOVE(name)                                                                                                            \
    name(name &&) = delete;                                                                                                      \
    name &operator=(name &&) = delete;

#define NO_COPY_NO_MOVE(name)                                                                                                    \
    NO_COPY(name);                                                                                                               \
    NO_MOVE(name);

#define CONST_AND_NON_CONST_GETTER(name, field)                                                                                  \
    inline auto &name() const {                                                                                                  \
        return field;                                                                                                            \
    }                                                                                                                            \
    inline auto &name() {                                                                                                        \
        return field;                                                                                                            \
    }

#include <source_location>
#define CUR_FUNC_NAME std::source_location::current().function_name()

#define GTEST_GROUP_CUSTOM Default_Test 
#define DEFINE_GTEST_CUSTOM(name) TEST(GTEST_GROUP_CUSTOM, name)

#endif