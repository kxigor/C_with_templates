#pragma once

#include <memory>
#include <optional>
#include <ostream>
#include <variant>

#include "config.hpp"
#include "token.hpp"

namespace scy {

/*=================== Forward declarations ===================*/
struct Expression;
struct Statement;
struct Declaration;

using ExprPtr = UniquePtrT<Expression>;
using StmtPtr = UniquePtrT<Statement>;
using DeclPtr = UniquePtrT<Declaration>;

/*======================= Expressions ========================*/
struct NumberExpr {
  Token value;
};

struct IdentifierExpr {
  Token name;
};

struct UnaryExpr {
  Token op;
  ExprPtr operand;
};

struct BinaryExpr {
  ExprPtr left;
  Token op;
  ExprPtr right;
};

struct AssignExpr {
  Token name;
  ExprPtr value;
};

struct CallExpr {
  Token callee;
  VectorT<ExprPtr> arguments;
};

struct GroupingExpr {
  ExprPtr expression;
};

using ExprVariant = VariantT<NumberExpr, IdentifierExpr, UnaryExpr, BinaryExpr,
                             AssignExpr, CallExpr, GroupingExpr>;

struct Expression {
  ExprVariant data;
  SourceLocation location;

  template <typename T>
  explicit Expression(T&& expr, SourceLocation loc)
      : data(std::forward<T>(expr)), location(loc) {}
};

/*======================== Statements ========================*/
struct ExpressionStmt {
  ExprPtr expression;
};

struct PrintStmt {
  ExprPtr expression;
};

struct ReturnStmt {
  Token keyword;
  OptionalT<ExprPtr> value;
};

struct BlockStmt {
  VectorT<StmtPtr> statements;
};

struct IfStmt {
  ExprPtr condition;
  StmtPtr then_branch;
  OptionalT<StmtPtr> else_branch;
};

struct VarDeclStmt {
  Token type;
  Token name;
  OptionalT<ExprPtr> initializer;
};

using StmtVariant = std::variant<ExpressionStmt, PrintStmt, ReturnStmt,
                                 BlockStmt, IfStmt, VarDeclStmt>;

struct Statement {
  StmtVariant data;
  SourceLocation location;

  template <typename T>
  explicit Statement(T&& stmt, SourceLocation loc)
      : data(std::forward<T>(stmt)), location(loc) {}
};

/*======================= Declarations =======================*/
struct Parameter {
  Token type;
  Token name;
};

struct FunctionDecl {
  Token return_type;
  Token name;
  VectorT<Parameter> params;
  VectorT<StmtPtr> body;
};

struct GlobalVarDecl {
  Token type;
  Token name;
  OptionalT<ExprPtr> initializer;
};

using DeclVariant = VariantT<FunctionDecl, GlobalVarDecl>;

struct Declaration {
  DeclVariant data;
  SourceLocation location;

  template <typename T>
  explicit Declaration(T&& decl, SourceLocation loc)
      : data(std::forward<T>(decl)), location(loc) {}
};

/*========================= AST Root =========================*/
struct Program {
  VectorT<DeclPtr> declarations;
};

/*======================= AST Printing =======================*/
void print_ast(std::ostream& os, const Program& program, int indent = 0);
void print_ast(std::ostream& os, const Declaration& decl, int indent = 0);
void print_ast(std::ostream& os, const Statement& stmt, int indent = 0);
void print_ast(std::ostream& os, const Expression& expr, int indent = 0);

}  // namespace scy
