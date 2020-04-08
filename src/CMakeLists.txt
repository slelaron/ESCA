
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

#set(CMAKE_BUILD_TYPE "Debug")
message(STATUS "Found LLVM: ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
#set(LLVM_LIBS ${LLVM_INCLUDE_DIRS}/../lib)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

#add_definitions(${LLVM_DEFINITIONS})
#add_definitions(${CLANG_DEFINITIONS})


add_library(ast SHARED
        AST/ASTWalker.cpp
        AST/ESCAASTConsumer.cpp
        AST/ESCAASTVisitor.cpp
        )

add_library(fsm SHARED
        FSM/FSM.cpp
        FSM/FSMStorage.cpp
        FSM/StateFSM.cpp
        FSM/TransitionFSM.cpp
        )

add_library(smt SHARED
        SMT/BinarySMT.cpp
        SMT/VariableSMT.cpp
        )

add_executable(ESCA main.cpp
        utils/DefectStorage.cpp
        Variable.cpp
        VersionedVariable.cpp
        #        Sort.cpp  -- почему-то нигде не используется
        #        patch/patchs.cpp
        )


target_link_libraries(ESCA
        LLVM
        ast
        fsm
        smt
        clangAST
        clangLex
        clangBasic
        clangFrontend
        clangTooling
        clangParse
        clangDriver
        )
