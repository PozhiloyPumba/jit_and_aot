cmake_minimum_required (VERSION 3.10)

project (IRGEN VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
# add_subdirectory(tests)