include_directories(../src)

add_executable(test run_tests.cpp)

target_link_libraries(test
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
