cmake_minimum_required(VERSION 3.10)
project(Calculator)

set(CMAKE_CXX_STANDARD 17)

# Добавляем CLI11 как подмодуль
add_subdirectory(lib/CLI11)

# Для Windows: определить _USE_MATH_DEFINES
if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif()

include_directories(include)

add_executable(calculator
    src/lexer.cpp
    src/parser.cpp
    src/evaluator.cpp
    src/main.cpp
)

target_link_libraries(calculator PRIVATE CLI11::CLI11)