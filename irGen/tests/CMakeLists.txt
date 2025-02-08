include(FetchContent)

FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set(TEST_EXE test)
add_executable(${TEST_EXE} dominatorTree.cpp loopTest.cpp livenessAnalysis.cpp)
target_link_libraries(${TEST_EXE} gtest_main ir_gen)

set_target_properties(${TEST_EXE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<0:>)

add_custom_target(run_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_EXE}$<IF:${WIN32},.exe,>
)

add_dependencies(run_tests ${TEST_EXE})