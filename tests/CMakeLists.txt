include_directories(../src)

add_executable(run_tests run_tests.cpp)

target_link_libraries(run_tests
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
