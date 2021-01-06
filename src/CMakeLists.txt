
add_subdirectory(AST)
add_subdirectory(CWE-467)
add_subdirectory(target)
add_subdirectory(SMT)
add_subdirectory(FSM)
add_subdirectory(utils)

add_executable(CWE467 main.cpp)


target_link_libraries(CWE467
        LLVM
        utils
        ast
        cwe-467
        target
        fsm
        smt
        z3
        clangAST
        clangLex
        clangBasic
        clangFrontend
        )
