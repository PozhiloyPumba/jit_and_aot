cmake_minimum_required (VERSION 3.10)

project (IRGEN VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

macro( add_recursive retVal dir ext1 excl)
    file(GLOB_RECURSE _tmp_list ${dir}/${ext1})
    set(${retval} "")
    foreach(f ${_tmp_list})
        if(NOT f MATCHES ${excl})
            list(APPEND ${retVal} ${f})
        endif()
    endforeach()
endmacro( add_recursive )

add_recursive(ALL_SOURCES_FILES "." "*.[ch]pp" "/build/")

find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)

add_custom_target(clang-force-format
    COMMAND ${CLANG_FORMAT_EXECUTABLE}
    -i
    -style=file
    --verbose
    ${ALL_SOURCES_FILES}
)

if(NOT WIN32)
    set(WIN32 0)
endif()

if(MSVC)
    add_compile_options("/W3" "$<$<CONFIG:RELEASE>:/O2>")
else()
    add_compile_options("-Wall" "-Wextra" "$<$<CONFIG:RELEASE>:-O2>")
endif()

include_directories(include)

add_subdirectory(src)
add_subdirectory(tests)