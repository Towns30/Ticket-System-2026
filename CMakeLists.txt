cmake_minimum_required(VERSION 3.14)

project(TicketSystem)

set(CMAKE_CXX_STANDARD 20)

include_directories("${PROJECT_SORCE_DIR}/include")

add_executable(program
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/Time.cpp
    src/UserManager.cpp
    src/TrainManager.cpp
    src/OrderManager.cpp
)

set(CMAKE_BUILD_TYPE Debug)