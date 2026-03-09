#include <cctype>
#include <include/config.hpp>
#include <include/lexer.hpp>
#include <include/token.hpp>

namespace scy {

namespace {
// clang-format off
const UmapT<StringViewT, TokenType> kKeywordsMap = {
    {"if",      TokenType::If     },
    {"else",    TokenType::Else   },
    {"void",    TokenType::Void   },
    {"int",     TokenType::Int    },
    {"print",   TokenType::Print  },
    {"return",  TokenType::Return }
};
// clang-format on

};  // namespace

VectorT<Token> Lexer::tokenize() {
  VectorT<Token> tokens;
  while (not is_at_end()) {
    tokens.emplace_back(next_token());
  }
  const Token kFinalToken = {
      .type = TokenType::Eof, .lexem = "", .location = location_};
  tokens.emplace_back(kFinalToken);
  return tokens;
}

[[nodiscard]] bool Lexer::is_at_end() const noexcept {
  return cursor_ >= source_.size();
}

[[nodiscard]] SymT Lexer::peek() const noexcept {
  return is_at_end() ? '\0' : source_[cursor_];
}

[[nodiscard]] SymT Lexer::peek_next() const noexcept {
  return (cursor_ + 1 >= source_.size()) ? '\0' : source_[cursor_ + 1];
}

SymT Lexer::advance() noexcept {
  const SymT kSym = source_[cursor_];
  ++cursor_;
  if (kSym == '\n') {
    ++location_.line;
    location_.column = 1;
  } else {
    ++location_.column;
  }
  return kSym;
}

void Lexer::skip_whitespace() noexcept {
  while (true) {
    const SymT kSym = peek();
    if (kSym == ' ' || kSym == '\r' || kSym == '\t' || kSym == '\n') {
      advance();
    } else {
      break;
    }
  }
}

Token Lexer::make_token(TokenType type) {
  // clang-format off
  const StringViewT kLexem = source_.substr(start_, cursor_ - start_);

  const SourceLocation kLocation = {
      .line   = location_.line, 
      .column = location_.column - static_cast<PosT>(kLexem.size())
  };

  return Token{
    .type     = type,
    .lexem    = kLexem,
    .location = kLocation
  };
  // clang-format on
}

Token Lexer::identifier() {
  while (std::isalnum(peek()) != 0 || peek() == '_') {
    advance();
  }

  const StringViewT kText = source_.substr(start_, cursor_ - start_);

  if (auto keyword = kKeywordsMap.find(kText); keyword != kKeywordsMap.end()) {
    return make_token(keyword->second);
  }

  return make_token(TokenType::Identifier);
}

Token Lexer::number() {
  while (std::isdigit(peek()) != 0) {
    advance();
  }
  return make_token(TokenType::Number);
}

Token Lexer::next_token() {
  skip_whitespace();
  start_ = cursor_;

  if (is_at_end()) {
    return make_token(TokenType::Eof);
  }

  const SymT kSym = advance();
  if (std::isalpha(kSym) != 0 || kSym == '_') {
    return identifier();
  }
  if (std::isdigit(kSym) != 0) {
    return number();
  }

  switch (kSym) {
    case '(':
      return make_token(TokenType::LParen);
    case ')':
      return make_token(TokenType::RParen);
    case '{':
      return make_token(TokenType::LBrace);
    case '}':
      return make_token(TokenType::RBrace);
    case ';':
      return make_token(TokenType::Semicolon);
    case ',':
      return make_token(TokenType::Comma);
    case '+':
      return make_token(TokenType::Plus);
    case '-':
      return make_token(TokenType::Minus);
    case '<':
      return make_token(TokenType::Less);
    case '>':
      return make_token(TokenType::Greater);
    case '=':
      if (peek() == '=') {
        advance();
        return make_token(TokenType::Equal);
      }
      return make_token(TokenType::Assign);
    case '!':
      if (peek() == '=') {
        advance();
        return make_token(TokenType::NotEqual);
      }
      return make_token(TokenType::Not);
    default:
      break;
  }

  return make_token(TokenType::Error);
}

}  // namespace scy