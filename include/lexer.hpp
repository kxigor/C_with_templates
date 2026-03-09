#pragma once

#include "config.hpp"
#include "token.hpp"

namespace scy {

class Lexer {
 public:
  explicit Lexer(StringView source) : source_(source) {}

  VectorT<Token> tokenize();

 private:
  [[nodiscard]] bool is_at_end() const noexcept;
  [[nodiscard]] SymT peek() const noexcept;
  [[nodiscard]] SymT peek_next() const noexcept;
  SymT advance() noexcept;

  void skip_whitespace() noexcept;

  Token next_token();
  Token make_token(TokenType type);
  Token error_token(StringView message);

  Token number();
  Token identifier();

  StringView source_;

  SourceLocation location_{.line = 1, .column = 1};

  PosT cursor_{0};
  PosT start_{0};
};

}  // namespace scy