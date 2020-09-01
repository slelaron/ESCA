include_directories(../FSM ../SMT)

add_library(target SHARED
        AnalyzeProcess.cpp
        Statements.cpp
        Context.cpp
        )