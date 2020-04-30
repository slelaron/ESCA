include_directories(../FSM ../SMT)

add_library(target SHARED
        Statements.cpp
        Context.cpp
        )