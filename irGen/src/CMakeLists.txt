set(SOURCES
    BB.cpp
    instr.cpp
    function.cpp
	IRBuilder.cpp
    loopAnalyzer.cpp
    livenessAnalysis.cpp
)
    
add_library(ir_gen ${SOURCES})

add_executable(main main.cpp)

target_link_libraries(main PUBLIC ir_gen)