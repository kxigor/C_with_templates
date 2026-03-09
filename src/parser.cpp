#include <include/parser.hpp>

namespace scy {

Program Parser::parse() {
  Program program;

  while (!is_at_end()) {
    try {
      if (auto decl = declaration()) {
        program.declarations.emplace_back(std::move(decl));
      }
    } catch (const ParseError& e) {
      errors_.push_back(e);
      synchronize();
    }
  }

  return program;
}

/*======================= Declarations =======================*/
DeclPtr Parser::declaration() {
  if (not is_type_token()) {
    throw error(peek(), "Expected type (int/void)");
  }

  Token type = advance();
  Token name = consume(TokenType::Identifier, "Expected identifier");

  if (match(TokenType::LParen)) {
    return function_declaration(type, name);
  }

  return variable_declaration(type, name);
}

DeclPtr Parser::function_declaration(Token type, Token name) {
  VectorT<Parameter> params;

  if (not check(TokenType::RParen)) {
    do {
      if (not is_type_token()) {
        throw error(peek(), "Expected parameter type");
      }
      Token param_type = advance();
      Token param_name =
          consume(TokenType::Identifier, "Expected parameter name");
      params.push_back({param_type, param_name});
    } while (match(TokenType::Comma));
  }

  consume(TokenType::RParen, "Expected ')' after parameters");
  consume(TokenType::LBrace, "Expected '{' before function body");

  VectorT<StmtPtr> body;
  while (!check(TokenType::RBrace) && !is_at_end()) {
    body.push_back(statement());
  }

  consume(TokenType::RBrace, "Expected '}' after function body");

  return MakeUnique<Declaration>(
      FunctionDecl{type, name, std::move(params), std::move(body)},
      type.location);
}

DeclPtr Parser::variable_declaration(Token type, Token name) {
  OptionalT<ExprPtr> initializer;

  if (match(TokenType::Assign)) {
    initializer = expression();
  }

  consume(TokenType::Semicolon, "Expected ';' after variable declaration");

  return MakeUnique<Declaration>(
      GlobalVarDecl{type, name, std::move(initializer)}, type.location);
}

// ==================== Statements ====================

StmtPtr Parser::statement() {
  if (match(TokenType::If)) {
    return if_statement();
  }
  if (match(TokenType::Return)) {
    return return_statement();
  }
  if (match(TokenType::Print)) {
    return print_statement();
  }
  if (match(TokenType::LBrace)) {
    return block_statement();
  }
  if (is_type_token()) {
    return var_decl_statement();
  }

  return expression_statement();
}

StmtPtr Parser::if_statement() {
  SourceLocation loc = previous().location;

  consume(TokenType::LParen, "Expected '(' after 'if'");
  ExprPtr condition = expression();
  consume(TokenType::RParen, "Expected ')' after condition");

  StmtPtr then_branch = statement();
  OptionalT<StmtPtr> else_branch;

  if (match(TokenType::Else)) {
    else_branch = statement();
  }

  return MakeUnique<Statement>(
      IfStmt{std::move(condition), std::move(then_branch),
             std::move(else_branch)},
      loc);
}

StmtPtr Parser::return_statement() {
  Token keyword = previous();
  OptionalT<ExprPtr> value;

  if (not check(TokenType::Semicolon)) {
    value = expression();
  }

  consume(TokenType::Semicolon, "Expected ';' after return");

  return MakeUnique<Statement>(ReturnStmt{keyword, std::move(value)},
                                     keyword.location);
}

StmtPtr Parser::print_statement() {
  SourceLocation loc = previous().location;
  ExprPtr value = expression();
  consume(TokenType::Semicolon, "Expected ';' after print");

  return MakeUnique<Statement>(PrintStmt{std::move(value)}, loc);
}

StmtPtr Parser::block_statement() {
  SourceLocation loc = previous().location;
  VectorT<StmtPtr> statements;

  while (!check(TokenType::RBrace) && !is_at_end()) {
    statements.push_back(statement());
  }

  consume(TokenType::RBrace, "Expected '}' after block");

  return MakeUnique<Statement>(BlockStmt{std::move(statements)}, loc);
}

StmtPtr Parser::expression_statement() {
  SourceLocation loc = peek().location;
  ExprPtr expr = expression();
  consume(TokenType::Semicolon, "Expected ';' after expression");

  return MakeUnique<Statement>(ExpressionStmt{std::move(expr)}, loc);
}

StmtPtr Parser::var_decl_statement() {
  Token type = advance();
  Token name = consume(TokenType::Identifier, "Expected variable name");

  OptionalT<ExprPtr> initializer;
  if (match(TokenType::Assign)) {
    initializer = expression();
  }

  consume(TokenType::Semicolon, "Expected ';' after variable declaration");

  return MakeUnique<Statement>(
      VarDeclStmt{type, name, std::move(initializer)}, type.location);
}

// ==================== Expressions ====================

ExprPtr Parser::expression() { return assignment(); }

ExprPtr Parser::assignment() {
  ExprPtr expr = equality();

  if (match(TokenType::Assign)) {
    Token equals = previous();

    // Check if left side is identifier
    if (auto* ident = std::get_if<IdentifierExpr>(&expr->data)) {
      ExprPtr value = assignment();
      return MakeUnique<Expression>(
          AssignExpr{ident->name, std::move(value)}, ident->name.location);
    }

    throw error(equals, "Invalid assignment target");
  }

  return expr;
}

ExprPtr Parser::equality() {
  ExprPtr expr = comparison();

  while (match(TokenType::Equal, TokenType::NotEqual)) {
    Token op = previous();
    ExprPtr right = comparison();
    expr = MakeUnique<Expression>(
        BinaryExpr{std::move(expr), op, std::move(right)}, op.location);
  }

  return expr;
}

ExprPtr Parser::comparison() {
  ExprPtr expr = term();

  while (match(TokenType::Less, TokenType::Greater)) {
    Token op = previous();
    ExprPtr right = term();
    expr = MakeUnique<Expression>(
        BinaryExpr{std::move(expr), op, std::move(right)}, op.location);
  }

  return expr;
}

ExprPtr Parser::term() {
  ExprPtr expr = unary();

  while (match(TokenType::Plus, TokenType::Minus)) {
    Token op = previous();
    ExprPtr right = unary();
    expr = MakeUnique<Expression>(
        BinaryExpr{std::move(expr), op, std::move(right)}, op.location);
  }

  return expr;
}

ExprPtr Parser::unary() {
  if (match(TokenType::Not, TokenType::Minus)) {
    Token op = previous();
    ExprPtr operand = unary();
    return MakeUnique<Expression>(UnaryExpr{op, std::move(operand)},
                                        op.location);
  }

  return primary();
}

ExprPtr Parser::primary() {
  if (match(TokenType::Number)) {
    Token token = previous();
    return MakeUnique<Expression>(NumberExpr{token}, token.location);
  }

  if (match(TokenType::Identifier)) {
    Token name = previous();

    // Check for function call
    if (match(TokenType::LParen)) {
      return call(name);
    }

    return MakeUnique<Expression>(IdentifierExpr{name}, name.location);
  }

  if (match(TokenType::LParen)) {
    SourceLocation loc = previous().location;
    ExprPtr expr = expression();
    consume(TokenType::RParen, "Expected ')' after expression");
    return MakeUnique<Expression>(GroupingExpr{std::move(expr)}, loc);
  }

  throw error(peek(), "Expected expression");
}

ExprPtr Parser::call(Token name) {
  VectorT<ExprPtr> arguments;

  if (not check(TokenType::RParen)) {
    do {
      arguments.push_back(expression());
    } while (match(TokenType::Comma));
  }

  consume(TokenType::RParen, "Expected ')' after arguments");

  return MakeUnique<Expression>(CallExpr{name, std::move(arguments)},
                                      name.location);
}

// ==================== Helper Methods ====================

bool Parser::is_at_end() const noexcept {
  return peek().type == TokenType::Eof;
}

Token Parser::peek() const noexcept { return tokens_[current_]; }

Token Parser::previous() const noexcept { return tokens_[current_ - 1]; }

Token Parser::advance() {
  if (not is_at_end()) {
    ++current_;
  }
  return previous();
}

bool Parser::check(TokenType type) const noexcept {
  if (is_at_end()) return false;
  return peek().type == type;
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  }
  return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
  if (check(type)) {
    return advance();
  }
  throw error(peek(), message);
}

bool Parser::is_type_token() const noexcept {
  return check(TokenType::Int) || check(TokenType::Void);
}

ParseError Parser::error(const Token& token, const std::string& message) {
  std::string full_message = "Line " + std::to_string(token.location.line) +
                             ", Col " + std::to_string(token.location.column) +
                             ": " + message;
  if (token.type != TokenType::Eof) {
    full_message += " (got '" + std::string(token.lexem) + "')";
  }
  return ParseError(full_message, token.location);
}

void Parser::synchronize() {
  advance();

  while (not is_at_end()) {
    if (previous().type == TokenType::Semicolon) return;

    switch (peek().type) {
      case TokenType::Int:
      case TokenType::Void:
      case TokenType::If:
      case TokenType::Return:
      case TokenType::Print:
        return;
      default:
        break;
    }

    advance();
  }
}

}  // namespace scy