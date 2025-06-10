#include <iostream>
#include <calculator_lib.h>
#include <CLI/CLI.hpp>

int main(int argc, char** argv) {
    CLI::App app{"RPN Calculator"};
    
    std::string expression;
    app.add_option("expression", expression, "Mathematical expression")->required();
    
    std::map<std::string, double> variables;
    app.add_option("--var,-v", variables, "Set variables (e.g., x=3.14)");
    
    CLI11_PARSE(app, argc, argv);
    
    try {
        Lexer lexer;
        auto tokens = lexer.tokenize(expression);
        
        Parser parser;
        auto rpnTokens = parser.parseToRPN(tokens);
        
        Evaluator evaluator;
        for (const auto& [name, value] : variables) {
            evaluator.setVariable(name, value);
        }
        
        double result = evaluator.evaluateRPN(rpnTokens);
        std::cout << "Result: " << result << std::endl;
        
    } catch (const CalcError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}