set(SOURCES
    BB.cpp
    instr.cpp
    function.cpp
	IRBuilder.cpp
    loopAnalyzer.cpp
    livenessAnalysis.cpp
    passes/peepholePass.cpp
    passes/constantFoldingPass.cpp
    passes/eliminateChecksPass.cpp
)

add_library(ir_gen ${SOURCES})

set(EXEC main)
add_executable(${EXEC} main.cpp)
target_link_libraries(${EXEC} PUBLIC ir_gen)

set_target_properties(${EXEC} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<0:>)

add_custom_target(run_main
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC}$<IF:${WIN32},.exe,>
)

add_dependencies(run_main ${EXEC})