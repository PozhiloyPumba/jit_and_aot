set(SOURCES
    BB.cpp
    instr.cpp
    graph.cpp
	IRBuilder.cpp
)

add_library(ir_gen ${SOURCES})

add_executable(test main.cpp)

target_link_libraries(test PUBLIC ir_gen)