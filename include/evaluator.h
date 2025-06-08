#pragma once
#include "tokens.h"
#include <vector>
#include <stack>
#include <map>
#include <string>

class Evaluator {
public:
    void setVariable(const std::string& name, double value);
    double evaluateRPN(const std::vector<Token>& rpnTokens);

private:
    void processOperator(const Token& token);
    void processFunction(const Token& token);

    std::stack<double> operandStack_;
    std::map<std::string, double> variables_;
};