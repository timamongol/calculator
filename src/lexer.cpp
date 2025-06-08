#include "lexer.h"
#include "error.h"
#include <cctype>
#include <cmath>
#include <map>

void Lexer::skipWhitespace() {
    while (pos_ < input_->size() && std::isspace((*input_)[pos_])) {
        ++pos_;
    }
}

void Lexer::tokenizeNumber() {
    size_t start = pos_;
    bool hasDecimal = false;
    
    while (pos_ < input_->size()) {
        char c = (*input_)[pos_];
        if (std::isdigit(c)) {
            ++pos_;
        } else if (c == '.' && !hasDecimal) {
            hasDecimal = true;
            ++pos_;
        } else {
            break;
        }
    }
    
    std::string numStr = input_->substr(start, pos_ - start);
    try {
        double value = std::stod(numStr);
        tokens_.push_back(Token(value));
    } catch (...) {
        throw SyntaxError("Invalid number: " + numStr);
    }
}

void Lexer::tokenizeIdentifier() {
    size_t start = pos_;
    while (pos_ < input_->size() && 
          (std::isalnum((*input_)[pos_]) || (*input_)[pos_] == '_')) {
        ++pos_;
    }
    
    std::string lexeme = input_->substr(start, pos_ - start);
    
    // Проверка констант
    if (lexeme == "PI") {
        tokens_.push_back(Token(TokenType::Constant, lexeme));
    } 
    // Проверка функций
    else if (lexeme == "sin" || lexeme == "cos" || lexeme == "!") {
        tokens_.push_back(Token(TokenType::Function, lexeme));
    }
    // Переменные
    else {
        tokens_.push_back(Token(TokenType::Variable, lexeme));
    }
}

void Lexer::tokenizeOperator() {
    char op = (*input_)[pos_++];
    tokens_.push_back(Token(TokenType::Operator, op));
}

void Lexer::tokenizeBracket() {
    char c = (*input_)[pos_];
    
    if (c == '(' || c == '[' || c == '{') {
        tokens_.push_back(Token(TokenType::LeftBracket, c));
    } else if (c == ')' || c == ']' || c == '}') {
        tokens_.push_back(Token(TokenType::RightBracket, c));
    }
    ++pos_;
}

std::vector<Token> Lexer::tokenize(const std::string& input) {
    input_ = &input;
    pos_ = 0;
    tokens_.clear();
    
    while (pos_ < input.size()) {
        skipWhitespace();
        if (pos_ >= input.size()) break;
        
        char c = input[pos_];
        if (std::isdigit(c)) {
            tokenizeNumber();
        }
        else if (std::isalpha(c) || c == '_') {
            tokenizeIdentifier();
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
            tokenizeOperator();
        }
        else if (c == '(' || c == '[' || c == '{' || 
                 c == ')' || c == ']' || c == '}') {
            tokenizeBracket();
        }
        else if (c == ',') {
            tokens_.push_back(Token(TokenType::Comma, c));
            ++pos_;
        }
        else {
            throw SyntaxError("Unexpected character: " + std::string(1, c));
        }
    }
    
    return tokens_;
}