cmake_minimum_required(VERSION 3.10)
project(calculator)

set(CMAKE_CXX_STANDARD 17)

# Добавляем CLI11 как подмодуль
add_subdirectory(lib/CLI11)
# НЕ ПО ТЗ, ДОБАВЛЕНО ДЛЯ ТЕСТОВ
add_subdirectory(lib/Catch2) 

# Для Windows: определить _USE_MATH_DEFINES
if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif()

#Добавляю библиотеку динамическую
#__________________________________________
add_library(${PROJECT_NAME}_lib SHARED
    lib/calculator_lib/src/lexer.cpp
    lib/calculator_lib/src/parser.cpp
    lib/calculator_lib/src/evaluator.cpp
)

add_library(${PROJECT_NAME}_lib::calculator_lib ALIAS ${PROJECT_NAME}_lib)

target_include_directories(${PROJECT_NAME}_lib
    PUBLIC
        lib/calculator_lib/include
)
#__________________________________________

add_executable(calculator
    src/main.cpp
)

target_link_libraries(calculator 
    PRIVATE 
        CLI11::CLI11
        ${PROJECT_NAME}_lib::calculator_lib
)

add_executable(test
    test/test.cpp
)
target_link_libraries(test 
    PRIVATE 
        Catch2::Catch2WithMain
        ${PROJECT_NAME}_lib::calculator_lib
)