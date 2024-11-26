find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRECTORIES})

add_executable(test dominatorTree.cpp loopTest.cpp livenessAnalysis.cpp)
target_link_libraries(test GTest::Main ir_gen)

add_executable(live livenessAnalysis.cpp)
target_link_libraries(live GTest::Main ir_gen)
