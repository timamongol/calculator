#pragma once
#include "tokens.h"
#include <vector>
#include <string>

class Lexer {
public:
    std::vector<Token> tokenize(const std::string& input);

private:
    void skipWhitespace();
    void tokenizeNumber();
    void tokenizeIdentifier();
    void tokenizeOperator();
    void tokenizeBracket();

    const std::string* input_;
    size_t pos_ = 0;
    std::vector<Token> tokens_;
};