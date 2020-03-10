
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

#set(LLVM_PATH /home/alex/extract_program/llvm-project/)
#set(LLVM_BUILD_PATH /home/alex/extract_program/llvm-project/build)



message(STATUS "Found LLVM: ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
set(LLVM_LIBS  ${LLVM_INCLUDE_DIRS}/../lib)

include_directories(${LLVM_INCLUDE_DIRS})
#include_directories(${CLANG_INCLUDE_DIRS})

add_definitions(${LLVM_DEFINITIONS})
#add_definitions(${CLANG_DEFINITIONS})

#target_link_libraries(ESCA ASTWalker.cpp)
#
#include_directories(ESCA INTERFACE ${LLVM_BUILD_PATH}/include;${LLVM_BUILD_PATH}/tools/clang/include;
#        ${LLVM_PATH}/llvm/include;${LLVM_PATH}/clang/include)
#link_directories(ESCA INTERFACE ${LLVM_BUILD_PATH}/lib)


add_executable(ESCA main.cpp
        ASTWalker.cpp
        BinarySMT.cpp
        DefectStorage.cpp
        ESCAASTConsumer.cpp
        ESCAASTVisitor.cpp
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
        clangAST
        clangFrontend
        LLVMSupport
        )
