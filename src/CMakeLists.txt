
add_subdirectory(AST)
add_subdirectory(target)
add_subdirectory(SMT)
add_subdirectory(FSM)

add_executable(ESCA main.cpp)


target_link_libraries(ESCA
        LLVM
        ast
        target
        fsm
        smt
        clangAST
        clangLex
        clangBasic
        clangFrontend
        )
