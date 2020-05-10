include_directories(../src)

add_executable(test run_tests.cpp)

target_link_libraries(test
        LLVM
        utils
        ast
        target
        fsm
        smt
        clangAST
        clangLex
        clangBasic
        clangFrontend
        )
