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

#include <source_location>
#define CUR_FUNC_NAME std::source_location::current().function_name()

#endif