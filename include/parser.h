#pragma once
#include "tokens.h"
#include <vector>
#include <stack>
#include <map>

class Parser {
public:
    std::vector<Token> parseToRPN(const std::vector<Token>& tokens);

private:
    int getPrecedence(const Token& token);
    bool isLeftAssociative(const Token& token);
    void handleOperator(const Token& token);
    void handleFunction(const Token& token);
    void handleLeftBracket(const Token& token);
    void handleRightBracket(const Token& token);

    std::vector<Token> output_;
    std::stack<Token> stack_;
    const std::vector<Token>* tokens_ = nullptr;
};