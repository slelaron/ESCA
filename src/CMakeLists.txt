
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

#set(CMAKE_BUILD_TYPE "Debug")
message(STATUS "Found LLVM: ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
set(LLVM_LIBS  ${LLVM_INCLUDE_DIRS}/../lib)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

#add_definitions(${LLVM_DEFINITIONS})
#add_definitions(${CLANG_DEFINITIONS})

add_executable(ESCA main.cpp
        ASTWalker.cpp
        ESCAASTConsumer.cpp
        ESCAASTVisitor.cpp
        BinarySMT.cpp
        DefectStorage.cpp
        ExecSolver.cpp
        FSM.cpp
        FSMStorage.cpp
        PathStorage.cpp
        Sort.cpp
        StateFSM.cpp
        TransitionFSM.cpp
        Variable.cpp
        VariableSMT.cpp
        VersionedVariable.cpp
        )


target_link_libraries(ESCA
        LLVM
        clangAST
        clangLex
        clangBasic
        clangFrontend
        clangTooling
        clangParse
        clangDriver
        )
