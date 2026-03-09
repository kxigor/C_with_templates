#pragma once

#include <stdexcept>
#include <string>
#include <utility>
#include <cstddef>

#include "ast.hpp"
#include "config.hpp"
#include "token.hpp"

namespace scy {

class ParseError : public std::runtime_error {
 public:
  ParseError(const std::string& message, SourceLocation location)
      : std::runtime_error(message), location_(location) {}

  [[nodiscard]] SourceLocation location() const noexcept { return location_; }

 private:
  SourceLocation location_;
};

class Parser {
 public:
  explicit Parser(VectorT<Token> tokens) : tokens_(std::move(tokens)) {}

  Program parse();

  [[nodiscard]] const VectorT<ParseError>& errors() const noexcept {
    return errors_;
  }

  [[nodiscard]] bool has_errors() const noexcept { return not errors_.empty(); }

 private:
  DeclPtr declaration();
  DeclPtr function_declaration(Token type, Token name);
  DeclPtr variable_declaration(Token type, Token name);

  StmtPtr statement();
  StmtPtr if_statement();
  StmtPtr return_statement();
  StmtPtr print_statement();
  StmtPtr block_statement();
  StmtPtr expression_statement();
  StmtPtr var_decl_statement();

  ExprPtr expression();
  ExprPtr assignment();
  ExprPtr equality();
  ExprPtr comparison();
  ExprPtr term();
  ExprPtr unary();
  ExprPtr primary();
  ExprPtr call(Token name);

  [[nodiscard]] bool is_at_end() const noexcept;
  [[nodiscard]] Token peek() const noexcept;
  [[nodiscard]] Token previous() const noexcept;
  Token advance();

  [[nodiscard]] bool check(TokenType type) const noexcept;
  bool match(TokenType type);

  template <typename... Types>
  bool match(TokenType first, Types... rest) {
    if (match(first)) return true;
    return match(rest...);
  }

  Token consume(TokenType type, const std::string& message);

  [[nodiscard]] bool is_type_token() const noexcept;

  ParseError error(const Token& token, const std::string& message);
  void synchronize();

  VectorT<Token> tokens_;
  std::size_t current_{0};
  VectorT<ParseError> errors_;
};

}  // namespace scy
