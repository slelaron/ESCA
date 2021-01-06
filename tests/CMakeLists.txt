include_directories(../src)

add_executable(run_tests run_tests.cpp)

target_link_libraries(run_tests
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

#add_executable(tests examples/test1.cpp)
#add_executable(tests examples/test2.cpp)
#add_executable(tests examples/test3.cpp)
add_executable(tests examples/test4.cpp)
#add_executable(tests examples/test5.cpp)
