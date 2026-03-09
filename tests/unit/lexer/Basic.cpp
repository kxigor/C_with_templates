#include <gtest/gtest.h>

#include <include/lexer.hpp>

namespace scy::test {

class LexerTest : public ::testing::Test {
 protected:
  void AssertToken(const Token& token, TokenType expected_type,
                   StringViewT expected_lexem, uint32_t line, uint32_t col) {
    EXPECT_EQ(token.type, expected_type)
        << "Mismatched type for lexem: " << expected_lexem;
    EXPECT_EQ(token.lexem, expected_lexem);
    EXPECT_EQ(token.location.line, line);
    EXPECT_EQ(token.location.column, col);
  }
};

TEST_F(LexerTest, SingleCharacterTokens) {
  Lexer lexer("(){},;+-<>=!");
  auto tokens = lexer.tokenize();

  ASSERT_EQ(tokens.size(), 13);
  AssertToken(tokens[0], TokenType::LParen, "(", 1, 1);
  AssertToken(tokens[1], TokenType::RParen, ")", 1, 2);
  AssertToken(tokens[2], TokenType::LBrace, "{", 1, 3);
  AssertToken(tokens[3], TokenType::RBrace, "}", 1, 4);
  AssertToken(tokens[4], TokenType::Comma, ",", 1, 5);
  AssertToken(tokens[5], TokenType::Semicolon, ";", 1, 6);
  AssertToken(tokens[6], TokenType::Plus, "+", 1, 7);
  AssertToken(tokens[7], TokenType::Minus, "-", 1, 8);
  AssertToken(tokens[8], TokenType::Less, "<", 1, 9);
  AssertToken(tokens[9], TokenType::Greater, ">", 1, 10);
  AssertToken(tokens[10], TokenType::Assign, "=", 1, 11);
  AssertToken(tokens[11], TokenType::Not, "!", 1, 12);
  EXPECT_EQ(tokens[12].type, TokenType::Eof);
}

TEST_F(LexerTest, CompositeOperators) {
  Lexer lexer("== != = !");
  auto tokens = lexer.tokenize();

  AssertToken(tokens[0], TokenType::Equal, "==", 1, 1);
  AssertToken(tokens[1], TokenType::NotEqual, "!=", 1, 4);
  AssertToken(tokens[2], TokenType::Assign, "=", 1, 7);
  AssertToken(tokens[3], TokenType::Not, "!", 1, 9);
}

TEST_F(LexerTest, IdentifiersAndKeywords) {
  Lexer lexer("if else void int print return _myVar var123 if_not_keyword");
  auto tokens = lexer.tokenize();

  AssertToken(tokens[0], TokenType::If, "if", 1, 1);
  AssertToken(tokens[1], TokenType::Else, "else", 1, 4);
  AssertToken(tokens[2], TokenType::Void, "void", 1, 9);
  AssertToken(tokens[3], TokenType::Int, "int", 1, 14);
  AssertToken(tokens[4], TokenType::Print, "print", 1, 18);
  AssertToken(tokens[5], TokenType::Return, "return", 1, 24);
  AssertToken(tokens[6], TokenType::Identifier, "_myVar", 1, 31);
  AssertToken(tokens[7], TokenType::Identifier, "var123", 1, 38);
  AssertToken(tokens[8], TokenType::Identifier, "if_not_keyword", 1, 45);
}

TEST_F(LexerTest, Numbers) {
  Lexer lexer("123 0 99999");
  auto tokens = lexer.tokenize();

  AssertToken(tokens[0], TokenType::Number, "123", 1, 1);
  AssertToken(tokens[1], TokenType::Number, "0", 1, 5);
  AssertToken(tokens[2], TokenType::Number, "99999", 1, 7);
}

TEST_F(LexerTest, LocationTracking) {
  Lexer lexer("int a = 10;\nvoid main()");
  auto tokens = lexer.tokenize();

  AssertToken(tokens[0], TokenType::Int, "int", 1, 1);
  AssertToken(tokens[5], TokenType::Void, "void", 2, 1);
  AssertToken(tokens[6], TokenType::Identifier, "main", 2, 6);
}

TEST_F(LexerTest, WhitespaceAndUnknown) {
  Lexer lexer("  \t\r\n  @  ");
  auto tokens = lexer.tokenize();

  AssertToken(tokens[0], TokenType::Error, "@", 2, 3);
  EXPECT_EQ(tokens[1].type, TokenType::Eof);
}

TEST_F(LexerTest, FullProgram) {
  StringViewT code =
      "int main() {\n"
      "  if (x == 10) {\n"
      "    print x;\n"
      "  }\n"
      "  return 0;\n"
      "}";
  Lexer lexer(code);
  auto tokens = lexer.tokenize();

  AssertToken(tokens[0], TokenType::Int, "int", 1, 1);
  AssertToken(tokens[4], TokenType::LBrace, "{", 1, 12);
  AssertToken(tokens[8], TokenType::Equal, "==", 2, 9);
  AssertToken(tokens[16], TokenType::Return, "return", 5, 3);
  EXPECT_EQ(tokens.back().type, TokenType::Eof);
}

}  // namespace scy::test