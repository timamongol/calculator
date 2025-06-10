#pragma once
#include <string>
#include <vector>

enum class TokenType {
    Number,       // Число
    Operator,     // +, -, *, /, ^
    Function,     // sin, cos, ! (унарные)
    Constant,     // PI
    Variable,     // x, y, z
    LeftBracket,  // ( [ {
    RightBracket, // ) ] }
    Comma         // ,
};

struct Token {
    TokenType type;
    std::string lexeme; // Текстовое представление
    double value;       // Для чисел

    Token(TokenType type, const std::string& lexeme) 
        : type(type), lexeme(lexeme), value(0) {}
    
    Token(TokenType type, char c)
        : type(type), lexeme(1, c), value(0) {}
    
    Token(double value)
        : type(TokenType::Number), lexeme(""), value(value) {}
};