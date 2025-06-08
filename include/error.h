#pragma once
#include <stdexcept>
#include <string>

class CalcError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class SyntaxError : public CalcError {
public:
    SyntaxError(const std::string& msg) : CalcError("Syntax error: " + msg) {}
};

class MathError : public CalcError {
public:
    MathError(const std::string& msg) : CalcError("Math error: " + msg) {}
};

class RuntimeError : public CalcError {
public:
    RuntimeError(const std::string& msg) : CalcError("Runtime error: " + msg) {}
};