#include <gtest/gtest.h>
#include <translator.hpp>
using namespace translator;

// tests dlya lexera
TEST(LexerTest, SingleDigitNumber) {
    auto tokens = lex("5");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_DOUBLE_EQ(tokens[0].value, 5.0);
}

TEST(LexerTest, MultiDigitNumber) {
    auto tokens = lex("123");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_DOUBLE_EQ(tokens[0].value, 123.0);
}

TEST(LexerTest, DecimalNumber) {
    auto tokens = lex("123.45");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_DOUBLE_EQ(tokens[0].value, 123.45);
}

TEST(LexerTest, ZeroInteger) {
    auto tokens = lex("0");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_DOUBLE_EQ(tokens[0].value, 0.0);
}

TEST(LexerTest, ZeroDecimal) {
    auto tokens = lex("0.5");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_DOUBLE_EQ(tokens[0].value, 0.5);
}

TEST(LexerTest, AllOperators) {
    auto tokens = lex("1+2-3*4/5");
    ASSERT_EQ(tokens.size(), 10);
    EXPECT_EQ(tokens[1].type, TokenType::PLUS);
    EXPECT_EQ(tokens[3].type, TokenType::MINUS);
    EXPECT_EQ(tokens[5].type, TokenType::MULTIPLY);
    EXPECT_EQ(tokens[7].type, TokenType::DIVIDE);
}

TEST(LexerTest, Parentheses) {
    auto tokens = lex("(1+2)");
    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, TokenType::LPAREN);
    EXPECT_EQ(tokens[4].type, TokenType::RPAREN);
}

TEST(LexerTest, WhitespaceError) {
    EXPECT_THROW(lex("1 + 2"), std::runtime_error);
}

TEST(LexerTest, UnknownCharacter) {
    EXPECT_THROW(lex("1&2"), std::runtime_error);
}

TEST(LexerTest, InvalidNumberEndingWithDot) {
    EXPECT_THROW(lex("5."), std::runtime_error);
}

// тут уже для парсера
TEST(ParserTest, SimpleNumber) {
    auto tokens = lex("5");
    EXPECT_NO_THROW(validateTokens(tokens));
}

TEST(ParserTest, SimpleAddition) {
    auto tokens = lex("1+2");
    EXPECT_NO_THROW(validateTokens(tokens));
}

TEST(ParserTest, MultipleOperations) {
    auto tokens = lex("1+2*3-4/5");
    EXPECT_NO_THROW(validateTokens(tokens));
}

TEST(ParserTest, ParenthesesSimple) {
    auto tokens = lex("(1+2)");
    EXPECT_NO_THROW(validateTokens(tokens));
}

TEST(ParserTest, ParenthesesNested) {
    auto tokens = lex("((1+2)*3)");
    EXPECT_NO_THROW(validateTokens(tokens));
}

TEST(ParserTest, MissingOperand) {
    auto tokens = lex("1+");
    EXPECT_THROW(validateTokens(tokens), std::runtime_error);
}

TEST(ParserTest, UnmatchedOpenParen) {
    auto tokens = lex("(1+2");
    EXPECT_THROW(validateTokens(tokens), std::runtime_error);
}

TEST(ParserTest, UnmatchedCloseParen) {
    auto tokens = lex("1+2)");
    EXPECT_THROW(validateTokens(tokens), std::runtime_error);
}

TEST(ParserTest, EmptyParentheses) {
    auto tokens = lex("()");
    EXPECT_THROW(validateTokens(tokens), std::runtime_error);
}

TEST(ParserTest, OperatorAtStart) {
    auto tokens = lex("+1");
    EXPECT_THROW(validateTokens(tokens), std::runtime_error);
}

TEST(ParserTest, DoubleOperator) {
    auto tokens = lex("124124++111");
    EXPECT_THROW(validateTokens(tokens), std::runtime_error);
}

// тесты перевода в польскую нотацию
TEST(RPNTest, SimpleNumber) {
    auto tokens = lex("5");
    auto rpn = toRPN(tokens);
    EXPECT_EQ(rpn[0].type, TokenType::NUMBER);
    EXPECT_DOUBLE_EQ(rpn[0].value, 5.0);
}

TEST(RPNTest, SimpleAddition) {
    auto tokens = lex("1+2");
    auto rpn = toRPN(tokens);
    ASSERT_EQ(rpn.size(), 3);
    EXPECT_EQ(rpn[0].type, TokenType::NUMBER);
    EXPECT_EQ(rpn[1].type, TokenType::NUMBER);
    EXPECT_EQ(rpn[2].type, TokenType::PLUS);
}

TEST(RPNTest, PrecedenceMultiplyBeforeAdd) {
    auto tokens = lex("1+2*3");
    auto rpn = toRPN(tokens);
    ASSERT_EQ(rpn.size(), 5);
    EXPECT_DOUBLE_EQ(rpn[0].value, 1.0);
    EXPECT_DOUBLE_EQ(rpn[1].value, 2.0);
    EXPECT_DOUBLE_EQ(rpn[2].value, 3.0);
    EXPECT_EQ(rpn[3].type, TokenType::MULTIPLY);
    EXPECT_EQ(rpn[4].type, TokenType::PLUS);
}

TEST(RPNTest, ParenthesesOverridePrecedence) {
    auto tokens = lex("(1+2)*3");
    auto rpn = toRPN(tokens);
    ASSERT_EQ(rpn.size(), 5);
    EXPECT_DOUBLE_EQ(rpn[0].value, 1.0);
    EXPECT_DOUBLE_EQ(rpn[1].value, 2.0);
    EXPECT_EQ(rpn[2].type, TokenType::PLUS);
    EXPECT_DOUBLE_EQ(rpn[3].value, 3.0);
    EXPECT_EQ(rpn[4].type, TokenType::MULTIPLY);
}

TEST(RPNTest, ComplexExpression) {
    auto tokens = lex("(1+2)*(3-4)");
    auto rpn = toRPN(tokens);
    ASSERT_EQ(rpn.size(), 7);
}

// результы
TEST(EvaluatorTest, SimpleNumber) {
    auto tokens = lex("5");
    auto rpn = toRPN(tokens);
    auto result = evalRPN(rpn);
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, 5.0);
}

TEST(EvaluatorTest, SimpleAddition) {
    auto tokens = lex("1+2");
    auto rpn = toRPN(tokens);
    auto result = evalRPN(rpn);
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, 3.0);
}

TEST(EvaluatorTest, SimpleSubtraction) {
    auto tokens = lex("5-3");
    auto rpn = toRPN(tokens);
    auto result = evalRPN(rpn);
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, 2.0);
}

TEST(EvaluatorTest, SimpleMultiplication) {
    auto tokens = lex("2*3");
    auto rpn = toRPN(tokens);
    auto result = evalRPN(rpn);
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, 6.0);
}

TEST(EvaluatorTest, SimpleDivision) {
    auto tokens = lex("8/4");
    auto rpn = toRPN(tokens);
    auto result = evalRPN(rpn);
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, 2.0);
}

TEST(EvaluatorTest, DivisionByZero) {
    auto tokens = lex("5/0");
    auto rpn = toRPN(tokens);
    auto result = evalRPN(rpn);
    EXPECT_FALSE(result.success);
}

TEST(EvaluatorTest, Precedence) {
    auto tokens = lex("1+2*3");
    auto rpn = toRPN(tokens);
    auto result = evalRPN(rpn);
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, 7.0);
}

TEST(EvaluatorTest, ParenthesesOverridePrecedence) {
    auto tokens = lex("(1+2)*3");
    auto rpn = toRPN(tokens);
    auto result = evalRPN(rpn);
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, 9.0);
}

TEST(EvaluatorTest, ComplexExpression) {
    auto tokens = lex("(1+2)*(3-4)");
    auto rpn = toRPN(tokens);
    auto result = evalRPN(rpn);
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, -3.0);
}

TEST(EvaluatorTest, DecimalNumbers) {
    auto tokens = lex("1.5+2.5");
    auto rpn = toRPN(tokens);
    auto result = evalRPN(rpn);
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, 4.0);
}

TEST(TranslatorTest, SimpleExpression) {
    auto result = computeExpression("1+2");
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, 3.0);
}

TEST(TranslatorTest, ComplexExpression) {
    auto result = computeExpression("(1+2)*3-4/2");
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, 7.0);
}

TEST(TranslatorTest, DecimalNumbers) {
    auto result = computeExpression("3.5+2.5");
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.value, 6.0);
}

TEST(TranslatorTest, LexicalError) {
    auto result = computeExpression("1 + 2");
    EXPECT_FALSE(result.success);
}

TEST(TranslatorTest, SyntaxError) {
    auto result = computeExpression("1++2");
    EXPECT_FALSE(result.success);
}

TEST(TranslatorTest, DivisionByZeroError) {
    auto result = computeExpression("1/0");
    EXPECT_FALSE(result.success);
}

TEST(TranslatorTest, UnmatchedParentheses) {
    auto result = computeExpression("(1+2");
    EXPECT_FALSE(result.success);
}

TEST(TranslatorTest, EmptyExpression) {
    auto result = computeExpression("");
    EXPECT_FALSE(result.success);
}