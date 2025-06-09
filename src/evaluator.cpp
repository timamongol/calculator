#include "evaluator.h"
#include "error.h"
#include <cmath>
#include <iostream>

void Evaluator::setVariable(const std::string& name, double value) {
    variables_[name] = value;
}

void Evaluator::processOperator(const Token& token) {
    if (operandStack_.size() < 2) {
        throw RuntimeError("Not enough operands for operator " + token.lexeme);
    }
    
    double right = operandStack_.top(); operandStack_.pop();
    double left = operandStack_.top(); operandStack_.pop();
    double result = 0;
    
    switch (token.lexeme[0]) {
        case '+': result = left + right; break;
        case '-': result = left - right; break;
        case '*': result = left * right; break;
        case '/': 
            if (right == 0) throw MathError("Division by zero");
            result = left / right;
            break;
        case '^': result = std::pow(left, right); break;
        default:
            throw RuntimeError("Unknown operator: " + token.lexeme);
    }
    
    operandStack_.push(result);
}

void Evaluator::processFunction(const Token& token) {
    if (operandStack_.empty()) {
        throw RuntimeError("Not enough operands for function " + token.lexeme);
    }
    
    double arg = operandStack_.top(); operandStack_.pop();
    double result = 0;
    
    if (token.lexeme == "sin") {
        result = std::sin(arg);
    } 
    else if (token.lexeme == "cos") {
        result = std::cos(arg);
    } 
    else if (token.lexeme == "!") {
        if (arg < 0 || std::floor(arg) != arg) {
            throw MathError("Factorial requires non-negative integer");
        }
        long fact = 1;
        for (int i = 2; i <= static_cast<int>(arg); ++i) {
            fact *= i;
        }

        result = static_cast<double>(fact);
    }
    else {
        throw RuntimeError("Unknown function: " + token.lexeme);
    }
    
    operandStack_.push(result);
}

double Evaluator::evaluateRPN(const std::vector<Token>& rpnTokens) {
    // Очищаем стек перед вычислением
    while (!operandStack_.empty()) operandStack_.pop();
    
    for (const auto& token : rpnTokens) {
        switch (token.type) {
            case TokenType::Number:
                operandStack_.push(token.value);
                break;
                
            case TokenType::Constant:
                if (token.lexeme == "PI") {
                    operandStack_.push(M_PI);
                } else {
                    throw RuntimeError("Unknown constant: " + token.lexeme);
                }
                break;
                
            case TokenType::Variable:
                if (variables_.find(token.lexeme) != variables_.end()) {
                    operandStack_.push(variables_[token.lexeme]);
                } else {
                    throw RuntimeError("Undefined variable: " + token.lexeme);
                }
                break;
                
            case TokenType::Operator:
                processOperator(token);
                break;
                
            case TokenType::Function:
                processFunction(token);
                break;
                
            default:
                throw RuntimeError("Unexpected token in RPN");
        }
    }
    
    if (operandStack_.size() != 1) {
        throw RuntimeError("Invalid expression: too many operands left");
    }
    
    return operandStack_.top();
}