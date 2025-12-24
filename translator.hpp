#pragma once
#include "stack.hpp"
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <cmath>

namespace translator {

enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LPAREN,
    RPAREN,
    END
};

struct Token {
    TokenType type;
    double value;
    Token(TokenType t = TokenType::END, double v = 0.0) : type(t), value(v) {}
};

enum class NumberState {
    START,
    EXP_DOT,
    EXP_N_OR_DOT,
    AFT_DOT,
    EXP_END,
    END,
    ERROR
};

inline bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

inline bool parseNumber(const std::string& s, size_t& i, std::string& outNumber) {
    NumberState state = NumberState::START;
    size_t j = i;
    bool sawDigit = false;
    
    while (j < s.size()) {
        char c = s[j];
        
        switch (state) {
            case NumberState::START:
                if (c == '0') {
                    state = NumberState::EXP_DOT;
                    sawDigit = true;
                    ++j;
                } else if (c >= '1' && c <= '9') {
                    state = NumberState::EXP_N_OR_DOT;
                    sawDigit = true;
                    ++j;
                } else {
                    state = NumberState::ERROR;
                }
                break;
                
            case NumberState::EXP_DOT:
                if (c == '.') {
                    state = NumberState::AFT_DOT;
                    ++j;
                } else {
                    state = NumberState::END;
                }
                break;
                
            case NumberState::EXP_N_OR_DOT:
                if (isDigit(c)) {
                    sawDigit = true;
                    ++j;
                } else if (c == '.') {
                    state = NumberState::AFT_DOT;
                    ++j;
                } else {
                    state = NumberState::END;
                }
                break;
                
            case NumberState::AFT_DOT:
                if (isDigit(c)) {
                    state = NumberState::EXP_END;
                    sawDigit = true;
                    ++j;
                } else {
                    state = NumberState::ERROR;
                }
                break;
                
            case NumberState::EXP_END:
                if (isDigit(c)) {
                    ++j;
                } else {
                    state = NumberState::END;
                }
                break;
                
            default:
                state = NumberState::ERROR;
                break;
        }
        
        if (state == NumberState::END || state == NumberState::ERROR) {
            break;
        }
    }
    
    if (state == NumberState::EXP_DOT ||
        state == NumberState::EXP_N_OR_DOT || 
        state == NumberState::EXP_END) {
        state = NumberState::END;
    }
    
    if (state != NumberState::END || !sawDigit) {
        return false;
    }
    
    outNumber = s.substr(i, j - i);
    i = j;
    return true;
}

inline std::vector<Token> lex(const std::string& expr) {
    std::vector<Token> tokens;
    size_t i = 0;
    const size_t n = expr.size();
    
    while (i < n) {
        char c = expr[i];

        // ну я запретил пробелы, могу себе позволить
        if (std::isspace(static_cast<unsigned char>(c))) {
            throw std::runtime_error("Whitespace not allowed in expression");
        }
        
        // Числа
        if (isDigit(c) || c == '.') {
            std::string numStr;
            size_t start = i;
            if (!parseNumber(expr, i, numStr)) {
                throw std::runtime_error("Invalid number format near position " + std::to_string(start));
            }
            tokens.emplace_back(TokenType::NUMBER, std::stod(numStr));
            continue;
        }
        
        // Операнкторы и скобки
        switch (c) {
            case '+':
                tokens.emplace_back(TokenType::PLUS);
                break;
            case '-':
                tokens.emplace_back(TokenType::MINUS);
                break;
            case '*':
                tokens.emplace_back(TokenType::MULTIPLY);
                break;
            case '/':
                tokens.emplace_back(TokenType::DIVIDE);
                break;
            case '(':
                tokens.emplace_back(TokenType::LPAREN);
                break;
            case ')':
                tokens.emplace_back(TokenType::RPAREN);
                break;
            default:
                throw std::runtime_error("Unknown character: '" + std::string(1, c) + "' at position " + std::to_string(i));
        }
        ++i;
    }
    
    tokens.emplace_back(TokenType::END);
    return tokens;
}

/*
 * грамматика:
 * Expression := Term (('+' | '-') Term)*
 * Term       := Factor (('*' | '/') Factor)*
 * Factor     := NUMBER | '(' Expression ')'
 */
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens_(tokens), pos_(0) {}
    
    bool validate(std::string& error) {
        pos_ = 0;
        error_ = "";
        
        if (!parseExpression()) {
            error = error_;
            return false;
        }
        
        if (tokens_[pos_].type != TokenType::END) {
            error = "Expected end of expression";
            return false;
        }
        
        return true;
    }
    
private:
    const std::vector<Token>& tokens_;
    size_t pos_;
    std::string error_;
    
    bool parseExpression() {
        if (!parseTerm()) return false;
        while (tokens_[pos_].type == TokenType::PLUS || 
               tokens_[pos_].type == TokenType::MINUS) {
            ++pos_;
            if (!parseTerm()) {
                error_ = "Expected term after operator";
                return false;
            }
        }
        return true;
    }
    
    bool parseTerm() {
        if (!parseFactor()) return false;
        while (tokens_[pos_].type == TokenType::MULTIPLY || 
               tokens_[pos_].type == TokenType::DIVIDE) {
            ++pos_;
            if (!parseFactor()) {
                error_ = "Expected factor after operator";
                return false;
            }
        }
        return true;
    }
    
    bool parseFactor() {
        if (tokens_[pos_].type == TokenType::NUMBER) {
            ++pos_;
            return true;
        }
        if (tokens_[pos_].type == TokenType::LPAREN) {
            ++pos_;
            if (!parseExpression()) return false;
            if (tokens_[pos_].type != TokenType::RPAREN) {
                error_ = "Expected closing parenthesis";
                return false;
            }
            ++pos_;
            return true;
        }
        
        error_ = "Expected number or opening parenthesis";
        return false;
    }
};

inline void validateTokens(const std::vector<Token>& tokens) {
    if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == TokenType::END)) {
        throw std::runtime_error("Empty expression");
    }
    
    std::string error;
    Parser parser(tokens);
    if (!parser.validate(error)) {
        throw std::runtime_error("Syntax error: " + error);
    }
}

inline int getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 1;
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
            return 2;
        default:
            return 0;
    }
}

inline bool isOperator(TokenType type) {
    return type == TokenType::PLUS || 
           type == TokenType::MINUS || 
           type == TokenType::MULTIPLY || 
           type == TokenType::DIVIDE;
}

inline std::vector<Token> toRPN(const std::vector<Token>& tokens) {
    std::vector<Token> output;
    ds::Stack<Token> opStack;
    
    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::NUMBER:
                output.push_back(token);
                break;
                
            case TokenType::PLUS:
            case TokenType::MINUS:
            case TokenType::MULTIPLY:
            case TokenType::DIVIDE:
                while (!opStack.empty() && 
                       opStack.top().type != TokenType::LPAREN &&
                       getPrecedence(opStack.top().type) >= getPrecedence(token.type)) {
                    output.push_back(opStack.top());
                    opStack.pop();
                }
                opStack.push(token);
                break;
                
            case TokenType::LPAREN:
                opStack.push(token);
                break;
                
            case TokenType::RPAREN:
                while (!opStack.empty() && opStack.top().type != TokenType::LPAREN) {
                    output.push_back(opStack.top());
                    opStack.pop();
                }
                
                if (opStack.empty()) {
                    throw std::runtime_error("Mismatched parentheses");
                }
                
                opStack.pop();
                break;
                
            case TokenType::END:
                while (!opStack.empty()) {
                    if (opStack.top().type == TokenType::LPAREN) {
                        throw std::runtime_error("Mismatched parentheses");
                    }
                    output.push_back(opStack.top());
                    opStack.pop();
                }
                break;
        }
    }
    
    return output;
}

struct EvaluationResult {
    bool success;
    double value;
    std::string error;
    
    EvaluationResult(double v) : success(true), value(v), error("") {}
    EvaluationResult(const std::string& err) : success(false), value(0.0), error(err) {}
};

inline EvaluationResult evalRPN(const std::vector<Token>& rpn) {
    ds::Stack<double> valueStack;
    
    for (const auto& token : rpn) {
        if (token.type == TokenType::NUMBER) {
            valueStack.push(token.value);
        } else if (isOperator(token.type)) {
            if (valueStack.size() < 2) {
                return EvaluationResult("Invalid expression: not enough operands");
            }
            
            double right = valueStack.top();
            valueStack.pop();
            double left = valueStack.top();
            valueStack.pop();
            
            double result = 0.0;
            
            switch (token.type) {
                case TokenType::PLUS:
                    result = left + right;
                    break;
                case TokenType::MINUS:
                    result = left - right;
                    break;
                case TokenType::MULTIPLY:
                    result = left * right;
                    break;
                case TokenType::DIVIDE:
                    if (std::abs(right) < 1e-10) {
                        return EvaluationResult("Division by zero");
                    }
                    result = left / right;
                    break;
                default:
                    return EvaluationResult("Unknown operator");
            }
            
            valueStack.push(result);
        }
    }
    
    if (valueStack.size() != 1) {
        return EvaluationResult("Invalid expression: too many operands");
    }
    
    return EvaluationResult(valueStack.top());
}

// приколы для вывода и отадки
struct TranslationResult {
    bool success;
    double value;
    std::string error;
    std::vector<Token> tokens;
    std::vector<Token> rpnTokens;
    
    TranslationResult(double v, const std::vector<Token>& t, const std::vector<Token>& rpn) 
        : success(true), value(v), error(""), tokens(t), rpnTokens(rpn) {}
    
    TranslationResult(const std::string& err) 
        : success(false), value(0.0), error(err) {}
};

inline TranslationResult computeExpression(const std::string& expr) {
    try {
        auto tokens = lex(expr);
        validateTokens(tokens);
        auto rpnTokens = toRPN(tokens);
        auto result = evalRPN(rpnTokens);
        if (!result.success) {
            return TranslationResult(result.error);
        }
        return TranslationResult(result.value, tokens, rpnTokens);
    } catch (const std::exception& e) {
        return TranslationResult(std::string("Error: ") + e.what());
    }
}

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::NUMBER: return "NUM";
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::MULTIPLY: return "*";
        case TokenType::DIVIDE: return "/";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::END: return "END";
        default: return "?";
    }
}

inline std::string tokensToString(const std::vector<Token>& tokens) {
    std::string result;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        
        if (token.type == TokenType::NUMBER) {
            result += std::to_string(token.value);
        } else if (token.type != TokenType::END) {
            result += tokenTypeToString(token.type);
        }
        
        if (i < tokens.size() - 1 && token.type != TokenType::END) {
            result += " ";
        }
    }
    return result;
}

} // namespace translator