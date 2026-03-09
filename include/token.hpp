#pragma once

#include <cstdint>

#include "config.hpp"

namespace scy {

// clang-format off
enum class TokenType : std::uint8_t {
  /*========================= Keywords =========================*/
  If, 
  Else, 
  Void, 
  Int, 
  Print, 
  Return,

  /*========================= Identity =========================*/
  Identifier, 
  Number,  

  /*======================== Operators =========================*/
  /* !  */ Not,
  /* == */ Equal,
  /* != */ NotEqual,
  /* <  */ Less,
  /* >  */ Greater,
  /* +  */ Plus,
  /* -  */ Minus,
  /* =  */ Assign, 

  /*======================= Punctuators ========================*/
  /* ; */ Semicolon,
  /* , */ Comma,
  /* ( */ LParen,
  /* ) */ RParen,
  /* { */ LBrace,
  /* } */ RBrace,
  
  /*=========================== Spec ===========================*/
  Eof,
  Error

};
// clang-format on

struct SourceLocation {
  PosT line{};
  PosT column{};
};

struct Token {
  TokenType type;
  StringViewT lexem;
  SourceLocation location;
};

};  // namespace scy