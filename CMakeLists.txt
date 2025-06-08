cmake_minimum_required(VERSION 3.10)
project(Calculator)

set(CMAKE_CXX_STANDARD 17)

# Добавляем CLI11 как подмодуль
add_subdirectory(lib/CLI11)
# НЕ ПО ТЗ, ДОБАВЛЕНО ДЛЯ ТЕСТОВ
add_subdirectory(lib/Catch2) 

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

add_executable(test
    test/test.cpp
    src/lexer.cpp
    src/parser.cpp
    src/evaluator.cpp
)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)