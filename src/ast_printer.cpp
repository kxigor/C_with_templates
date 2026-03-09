#include <include/ast.hpp>

namespace scy {

namespace {

std::string indent_str(std::size_t indent) {
  return std::string(indent * 2, ' ');
}

}  // namespace

void print_ast(std::ostream& os, const Program& program, int indent) {
  os << indent_str(indent) << "Program\n";
  for (const auto& decl : program.declarations) {
    print_ast(os, *decl, indent + 1);
  }
}

void print_ast(std::ostream& os, const Declaration& decl, int indent) {
  std::visit(
      [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, FunctionDecl>) {
          os << indent_str(indent) << "FunctionDecl: " << arg.return_type.lexem
             << " " << arg.name.lexem << "(";

          for (std::size_t i = 0; i < arg.params.size(); ++i) {
            if (i > 0) os << ", ";
            os << arg.params[i].type.lexem << " " << arg.params[i].name.lexem;
          }
          os << ")\n";

          for (const auto& stmt : arg.body) {
            print_ast(os, *stmt, indent + 1);
          }

        } else if constexpr (std::is_same_v<T, GlobalVarDecl>) {
          os << indent_str(indent) << "GlobalVarDecl: " << arg.type.lexem << " "
             << arg.name.lexem;
          if (arg.initializer) {
            os << " =\n";
            print_ast(os, **arg.initializer, indent + 1);
          } else {
            os << "\n";
          }
        }
      },
      decl.data);
}

void print_ast(std::ostream& os, const Statement& stmt, int indent) {
  std::visit(
      [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, ExpressionStmt>) {
          os << indent_str(indent) << "ExpressionStmt\n";
          print_ast(os, *arg.expression, indent + 1);

        } else if constexpr (std::is_same_v<T, PrintStmt>) {
          os << indent_str(indent) << "PrintStmt\n";
          print_ast(os, *arg.expression, indent + 1);

        } else if constexpr (std::is_same_v<T, ReturnStmt>) {
          os << indent_str(indent) << "ReturnStmt\n";
          if (arg.value) {
            print_ast(os, **arg.value, indent + 1);
          }

        } else if constexpr (std::is_same_v<T, BlockStmt>) {
          os << indent_str(indent) << "BlockStmt\n";
          for (const auto& s : arg.statements) {
            print_ast(os, *s, indent + 1);
          }

        } else if constexpr (std::is_same_v<T, IfStmt>) {
          os << indent_str(indent) << "IfStmt\n";
          os << indent_str(indent + 1) << "Condition:\n";
          print_ast(os, *arg.condition, indent + 2);
          os << indent_str(indent + 1) << "Then:\n";
          print_ast(os, *arg.then_branch, indent + 2);
          if (arg.else_branch) {
            os << indent_str(indent + 1) << "Else:\n";
            print_ast(os, **arg.else_branch, indent + 2);
          }

        } else if constexpr (std::is_same_v<T, VarDeclStmt>) {
          os << indent_str(indent) << "VarDeclStmt: " << arg.type.lexem << " "
             << arg.name.lexem;
          if (arg.initializer) {
            os << " =\n";
            print_ast(os, **arg.initializer, indent + 1);
          } else {
            os << "\n";
          }
        }
      },
      stmt.data);
}

void print_ast(std::ostream& os, const Expression& expr, int indent) {
  std::visit(
      [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, NumberExpr>) {
          os << indent_str(indent) << "Number: " << arg.value.lexem << "\n";

        } else if constexpr (std::is_same_v<T, IdentifierExpr>) {
          os << indent_str(indent) << "Identifier: " << arg.name.lexem << "\n";

        } else if constexpr (std::is_same_v<T, UnaryExpr>) {
          os << indent_str(indent) << "UnaryExpr: " << arg.op.lexem << "\n";
          print_ast(os, *arg.operand, indent + 1);

        } else if constexpr (std::is_same_v<T, BinaryExpr>) {
          os << indent_str(indent) << "BinaryExpr: " << arg.op.lexem << "\n";
          print_ast(os, *arg.left, indent + 1);
          print_ast(os, *arg.right, indent + 1);

        } else if constexpr (std::is_same_v<T, AssignExpr>) {
          os << indent_str(indent) << "AssignExpr: " << arg.name.lexem << "\n";
          print_ast(os, *arg.value, indent + 1);

        } else if constexpr (std::is_same_v<T, CallExpr>) {
          os << indent_str(indent) << "CallExpr: " << arg.callee.lexem << "\n";
          for (const auto& a : arg.arguments) {
            print_ast(os, *a, indent + 1);
          }

        } else if constexpr (std::is_same_v<T, GroupingExpr>) {
          os << indent_str(indent) << "GroupingExpr\n";
          print_ast(os, *arg.expression, indent + 1);
        }
      },
      expr.data);
}

}  // namespace scy