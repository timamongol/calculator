#include "parser.h"
#include "error.h"
#include <map>
#include <cctype>

int Parser::getPrecedence(const Token& token) {
    static const std::map<std::string, int> precedence = {
        {"!", 5}, {"^", 4},
        {"*", 3}, {"/", 3},
        {"+", 2}, {"-", 2},
        {"sin", 5}, {"cos", 5}
    };
    
    if (token.type == TokenType::Function) {
        return precedence.at(token.lexeme);
    }
    
    auto it = precedence.find(token.lexeme);
    return (it != precedence.end()) ? it->second : 0;
}

bool Parser::isLeftAssociative(const Token& token) {
    return (token.lexeme != "^" && token.lexeme != "!");
}

void Parser::handleOperator(const Token& token) {
    while (!stack_.empty()) {
        Token top = stack_.top();
        
        if ((top.type == TokenType::Operator || top.type == TokenType::Function) &&
            (getPrecedence(top) > getPrecedence(token) || 
             (getPrecedence(top) == getPrecedence(token) && isLeftAssociative(token)))) {
            output_.push_back(top);
            stack_.pop();
        } else {
            break;
        }
    }
    stack_.push(token);
}

void Parser::handleFunction(const Token& token) {
    stack_.push(token);
}

void Parser::handleLeftBracket(const Token& token) {
    stack_.push(token);
}

void Parser::handleRightBracket(const Token& token) {
    std::map<char, char> matching = {
        {')', '('}, {']', '['}, {'}', '{'}
    };
    char openBracket = matching.at(token.lexeme[0]);
    
    bool found = false;
    while (!stack_.empty()) {
        Token top = stack_.top();
        stack_.pop();
        
        if (top.type == TokenType::LeftBracket && top.lexeme[0] == openBracket) {
            found = true;
            break;
        }
        output_.push_back(top);
    }
    
    if (!found) {
        throw SyntaxError("Mismatched brackets");
    }
    
    if (!stack_.empty() && stack_.top().type == TokenType::Function) {
        output_.push_back(stack_.top());
        stack_.pop();
    }
}

std::vector<Token> Parser::parseToRPN(const std::vector<Token>& tokens) {
    tokens_ = &tokens;
    output_.clear();
    while (!stack_.empty()) stack_.pop();
    
    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::Number:
            case TokenType::Constant:
            case TokenType::Variable:
                output_.push_back(token);
                break;
                
            case TokenType::Function:
                handleFunction(token);
                break;
                
            case TokenType::Operator:
                handleOperator(token);
                break;
                
            case TokenType::LeftBracket:
                handleLeftBracket(token);
                break;
                
            case TokenType::RightBracket:
                handleRightBracket(token);
                break;
                
            case TokenType::Comma:
                // Пока не поддерживаем функции с несколькими аргументами
                throw SyntaxError("Comma not supported");
                break;
                
            default:
                throw SyntaxError("Unknown token type");
        }
    }
    
    while (!stack_.empty()) {
        Token top = stack_.top();
        stack_.pop();
        
        if (top.type == TokenType::LeftBracket) {
            throw SyntaxError("Mismatched brackets");
        }
        output_.push_back(top);
    }
    
    return output_;
}