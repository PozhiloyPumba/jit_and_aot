find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRECTORIES})

add_executable(test dominatorTree.cpp)
target_link_libraries(test GTest::Main ir_gen)