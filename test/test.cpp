#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <calculator_lib.h>
#include <cmath>

using Catch::Approx;

TEST_CASE("Lexer tokenization", "[lexer]") {
    Lexer lexer;
    
    SECTION("Numbers") {
        auto tokens = lexer.tokenize("3.14 42");
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type == TokenType::Number);
        CHECK(tokens[0].value == 3.14);
        CHECK(tokens[1].type == TokenType::Number);
        CHECK(tokens[1].value == 42);
    }
    
    SECTION("Operators") {
        auto tokens = lexer.tokenize("+ - * / ^");
        REQUIRE(tokens.size() == 5);
        CHECK(tokens[0].lexeme == "+");
        CHECK(tokens[1].lexeme == "unary_minus");
        CHECK(tokens[2].lexeme == "*");
        CHECK(tokens[3].lexeme == "/");
        CHECK(tokens[4].lexeme == "^");
    }
    
    SECTION("Functions and constants") {
        auto tokens = lexer.tokenize("sin cos PI x !");
        REQUIRE(tokens.size() == 5);
        CHECK(tokens[0].type == TokenType::Function);
        CHECK(tokens[0].lexeme == "sin");
        CHECK(tokens[1].type == TokenType::Function);
        CHECK(tokens[1].lexeme == "cos");
        CHECK(tokens[2].type == TokenType::Constant);
        CHECK(tokens[2].lexeme == "PI");
        CHECK(tokens[3].type == TokenType::Variable);
        CHECK(tokens[3].lexeme == "x");
        CHECK(tokens[4].type == TokenType::Function);
        CHECK(tokens[4].lexeme == "!");
    }
    
    SECTION("Brackets") {
        auto tokens = lexer.tokenize("( ) [ ] { }");
        REQUIRE(tokens.size() == 6);
        CHECK(tokens[0].lexeme == "(");
        CHECK(tokens[0].type == TokenType::LeftBracket);
        CHECK(tokens[1].lexeme == ")");
        CHECK(tokens[1].type == TokenType::RightBracket);
        CHECK(tokens[2].lexeme == "[");
        CHECK(tokens[3].lexeme == "]");
        CHECK(tokens[4].lexeme == "{");
        CHECK(tokens[5].lexeme == "}");
    }
    
    SECTION("Invalid characters") {
        REQUIRE_THROWS_AS(lexer.tokenize("3 @ 4"), SyntaxError);
    }
}

TEST_CASE("Parser RPN conversion", "[parser]") {
    Lexer lexer;
    Parser parser;
    
    auto toRPNString = [&](const std::string& expr) {
        auto tokens = lexer.tokenize(expr);
        auto rpn = parser.parseToRPN(tokens);
        std::string result;
        for (const auto& token : rpn) {
            if (!result.empty()) result += " ";
            if (token.type == TokenType::Number) {
                if (std::floor(token.value) == token.value) {
                    result += std::to_string(static_cast<int>(token.value));
                } else { 
                    result += std::to_string(token.value);
                }
            } else {
                result += token.lexeme;
            }
        }
        return result;
    };
    
    SECTION("Basic expressions") {
        CHECK(toRPNString("3 + 4") == "3 4 +");
        CHECK(toRPNString("3 + 4 * 2") == "3 4 2 * +");
        CHECK(toRPNString("3 * (4 + 2)") == "3 4 2 + *");
    }
    
    SECTION("Functions") {
        CHECK(toRPNString("sin(0)") == "0 sin");
        CHECK(toRPNString("cos(PI)") == "PI cos");
        CHECK(toRPNString("2 * sin(PI/2)") == "2 PI 2 / sin *");
        // CHECK(toRPNString("2!") == "2 !");
    }
    
    SECTION("Exponentiation") {
        CHECK(toRPNString("2 ^ 3") == "2 3 ^");
        CHECK(toRPNString("2 ^ 3 ^ 2") == "2 3 2 ^ ^");
    }
    
    SECTION("Brackets matching") {
        CHECK(toRPNString("(2 + 3) * 4") == "2 3 + 4 *");
        REQUIRE_THROWS_AS(parser.parseToRPN(lexer.tokenize("(2 + 3]")), SyntaxError);
        REQUIRE_THROWS_AS(parser.parseToRPN(lexer.tokenize("2 + 3)")), SyntaxError);
    }
}

TEST_CASE("Evaluator computation", "[evaluator]") {
    Lexer lexer;
    Parser parser;
    Evaluator eval;
    
    auto evalExpr = [&](const std::string& expr, 
                        const std::map<std::string, double>& vars = {}) -> double {
        for (const auto& [name, value] : vars) {
            eval.setVariable(name, value);
        }
        auto tokens = lexer.tokenize(expr);
        auto rpn = parser.parseToRPN(tokens);
        return eval.evaluateRPN(rpn);
    };
    
    SECTION("Basic arithmetic") {
        CHECK(evalExpr("2 + 2") == 4);
        CHECK(evalExpr("5 - 3") == 2);
        CHECK(evalExpr("4 * 3") == 12);
        CHECK(evalExpr("10 / 2") == 5);
        CHECK(evalExpr("2 ^ 3") == 8);
    }
    
    SECTION("Constants") {
        CHECK(evalExpr("PI") == M_PI);
        CHECK(evalExpr("2 * PI") == 2 * M_PI);
    }
    
    SECTION("Functions") {
        CHECK(evalExpr("sin(0)") == 0);
        CHECK(evalExpr("cos(0)") == 1);
        CHECK(evalExpr("sin(PI/2)") == Approx(1).margin(1e-5));
        CHECK(evalExpr("5!") == 120);
    }
    
    SECTION("Variables") {
        CHECK(evalExpr("x", {{"x", 5}}) == 5);
        CHECK(evalExpr("x * y", {{"x", 3}, {"y", 4}}) == 12);
        CHECK(evalExpr("sin(x)", {{"x", M_PI/2}}) == Approx(1).margin(1e-5));
    }
    
    SECTION("Complex expressions") {
        CHECK(evalExpr("2 + 3 * 4") == 14);
        CHECK(evalExpr("(2 + 3) * 4") == 20);
        CHECK(evalExpr("3 * sin(PI/2) + cos(0)") == Approx(4).margin(1e-5));
        CHECK(evalExpr("2 ^ (3 + 1)") == 16);
    }
    
    SECTION("Error handling") {
        SECTION("Division by zero") {
            REQUIRE_THROWS_AS(evalExpr("1 / 0"), MathError);
        }

        SECTION("Factorial negative") {
            REQUIRE_THROWS_AS(evalExpr("(-1)!"), MathError);
        }
        
        SECTION("Factorial non-integer") {
            REQUIRE_THROWS_AS(evalExpr("3.5!"), MathError);
        }
        
        SECTION("Undefined variable") {
            REQUIRE_THROWS_AS(evalExpr("x"), RuntimeError);
        }
        
        SECTION("Mismatched brackets") {
            REQUIRE_THROWS_AS(evalExpr("(2 + 3"), SyntaxError);
        }
    }
}

TEST_CASE("Negative numbers", "[evaluator]") {
    Lexer lexer;
    Parser parser;
    Evaluator eval;

    auto evalExpr = [&](const std::string& expr, 
                        const std::map<std::string, double>& vars = {}) -> double {
        for (const auto& [name, value] : vars) {
            eval.setVariable(name, value);
        }
        auto tokens = lexer.tokenize(expr);
        auto rpn = parser.parseToRPN(tokens);
        return eval.evaluateRPN(rpn);
    };

    SECTION("Basic negatives") {
        CHECK(evalExpr("-5") == -5);
        CHECK(evalExpr("-5 + 3") == -2);
        CHECK(evalExpr("3 + -5") == -2);
        CHECK(evalExpr("2 * -3") == -6);
    }
    
    SECTION("Complex expressions") {
        CHECK(evalExpr("2 ^ -2") == 0.25);
        CHECK(evalExpr("sin(-PI/2)") == -1);
        CHECK(evalExpr("-3!") == -6);
        CHECK(evalExpr("(-5) ^ 2") == 25);
    }
    
    SECTION("Multiple unary") {
        CHECK(evalExpr("--5") == 5);
        CHECK(evalExpr("---5") == -5);
        CHECK(evalExpr("-(3 + 2)") == -5);
    }
}

TEST_CASE("Full pipeline", "[integration]") {
    Lexer lexer;
    Parser parser;
    Evaluator eval;
    
    SECTION("Example from specification") {
        auto tokens = lexer.tokenize("2 + sin(x) / {3 + cos(x)} * PI");
        auto rpn = parser.parseToRPN(tokens);
        eval.setVariable("x", M_PI);
        
        // Calculate expected value
        double x = M_PI;
        double expected = 2 + std::sin(x) / (3 + std::cos(x)) * M_PI;
        
        CHECK(eval.evaluateRPN(rpn) == Approx(expected).margin(1e-5));
    }
    
    SECTION("Complex expression with variables") {
        eval.setVariable("a", 2);
        eval.setVariable("b", 3);
        eval.setVariable("c", 4);
        
        auto tokens = lexer.tokenize("(a + b) * c / (a ^ b)");
        auto rpn = parser.parseToRPN(tokens);
        
        double result = eval.evaluateRPN(rpn);
        double expected = (2 + 3) * 4 / std::pow(2, 3);
        CHECK(result == Approx(expected).margin(1e-5));
    }
}