#include <include/ast_visualizer.hpp>

namespace scy {

void AstVisualizer::generate_dot(const Program& program) {
  os_ << "digraph AST {\n";
  os_ << "  node [fontname=\"Helvetica\", fontsize=10];\n";
  os_ << "  edge [fontname=\"Helvetica\", fontsize=9];\n\n";

  visit_program(program);

  os_ << "}\n";
}

std::size_t AstVisualizer::next_id() { return id_counter_++; }

void AstVisualizer::add_node(std::size_t id, const std::string& label,
                             const std::string& shape) {
  os_ << "  node" << id << " [label=\"" << label << "\", shape=" << shape
      << "];\n";
}

void AstVisualizer::add_edge(std::size_t from, std::size_t to,
                             const std::string& label) {
  os_ << "  node" << from << " -> node" << to;
  if (!label.empty()) {
    os_ << " [label=\"" << label << "\"]";
  }
  os_ << ";\n";
}

std::size_t AstVisualizer::visit_program(const Program& program) {
  std::size_t id = next_id();
  add_node(id, "Program", "ellipse");

  for (const auto& decl : program.declarations) {
    std::size_t child_id = visit_declaration(*decl);
    add_edge(id, child_id);
  }

  return id;
}

std::size_t AstVisualizer::visit_declaration(const Declaration& decl) {
  return std::visit(
      [&](auto&& arg) -> std::size_t {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, FunctionDecl>) {
          std::size_t id = next_id();
          std::string label = "Function\\n" +
                              std::string(arg.return_type.lexem) + " " +
                              std::string(arg.name.lexem) + "(";
          for (std::size_t i = 0; i < arg.params.size(); ++i) {
            if (i > 0) label += ", ";
            label += std::string(arg.params[i].type.lexem) + " " +
                     std::string(arg.params[i].name.lexem);
          }
          label += ")";
          add_node(id, label, "box");

          for (const auto& stmt : arg.body) {
            std::size_t child_id = visit_statement(*stmt);
            add_edge(id, child_id);
          }

          return id;

        } else if constexpr (std::is_same_v<T, GlobalVarDecl>) {
          std::size_t id = next_id();
          std::string label = "GlobalVar\\n" + std::string(arg.type.lexem) +
                              " " + std::string(arg.name.lexem);
          add_node(id, label, "box");

          if (arg.initializer) {
            std::size_t init_id = visit_expression(**arg.initializer);
            add_edge(id, init_id, "init");
          }

          return id;
        }

        return 0;
      },
      decl.data);
}

std::size_t AstVisualizer::visit_statement(const Statement& stmt) {
  return std::visit(
      [&](auto&& arg) -> std::size_t {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, ExpressionStmt>) {
          std::size_t id = next_id();
          add_node(id, "ExprStmt", "box");
          std::size_t expr_id = visit_expression(*arg.expression);
          add_edge(id, expr_id);
          return id;

        } else if constexpr (std::is_same_v<T, PrintStmt>) {
          std::size_t id = next_id();
          add_node(id, "Print", "box");
          std::size_t expr_id = visit_expression(*arg.expression);
          add_edge(id, expr_id);
          return id;

        } else if constexpr (std::is_same_v<T, ReturnStmt>) {
          std::size_t id = next_id();
          add_node(id, "Return", "box");
          if (arg.value) {
            std::size_t val_id = visit_expression(**arg.value);
            add_edge(id, val_id);
          }
          return id;

        } else if constexpr (std::is_same_v<T, BlockStmt>) {
          std::size_t id = next_id();
          add_node(id, "Block", "box");
          for (const auto& s : arg.statements) {
            std::size_t child_id = visit_statement(*s);
            add_edge(id, child_id);
          }
          return id;

        } else if constexpr (std::is_same_v<T, IfStmt>) {
          std::size_t id = next_id();
          add_node(id, "If", "diamond");

          std::size_t cond_id = visit_expression(*arg.condition);
          add_edge(id, cond_id, "cond");

          std::size_t then_id = visit_statement(*arg.then_branch);
          add_edge(id, then_id, "then");

          if (arg.else_branch) {
            std::size_t else_id = visit_statement(**arg.else_branch);
            add_edge(id, else_id, "else");
          }

          return id;

        } else if constexpr (std::is_same_v<T, VarDeclStmt>) {
          std::size_t id = next_id();
          std::string label = "VarDecl\\n" + std::string(arg.type.lexem) + " " +
                              std::string(arg.name.lexem);
          add_node(id, label, "box");

          if (arg.initializer) {
            std::size_t init_id = visit_expression(**arg.initializer);
            add_edge(id, init_id, "init");
          }

          return id;
        }

        return 0;
      },
      stmt.data);
}

std::size_t AstVisualizer::visit_expression(const Expression& expr) {
  return std::visit(
      [&](auto&& arg) -> std::size_t {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, NumberExpr>) {
          std::size_t id = next_id();
          add_node(id, std::string(arg.value.lexem), "ellipse");
          return id;

        } else if constexpr (std::is_same_v<T, IdentifierExpr>) {
          std::size_t id = next_id();
          add_node(id, std::string(arg.name.lexem), "ellipse");
          return id;

        } else if constexpr (std::is_same_v<T, UnaryExpr>) {
          std::size_t id = next_id();
          add_node(id, "Unary " + std::string(arg.op.lexem), "circle");
          std::size_t operand_id = visit_expression(*arg.operand);
          add_edge(id, operand_id);
          return id;

        } else if constexpr (std::is_same_v<T, BinaryExpr>) {
          std::size_t id = next_id();
          add_node(id, std::string(arg.op.lexem), "circle");
          std::size_t left_id = visit_expression(*arg.left);
          std::size_t right_id = visit_expression(*arg.right);
          add_edge(id, left_id, "L");
          add_edge(id, right_id, "R");
          return id;

        } else if constexpr (std::is_same_v<T, AssignExpr>) {
          std::size_t id = next_id();
          add_node(id, "Assign", "circle");

          std::size_t name_id = next_id();
          add_node(name_id, std::string(arg.name.lexem), "ellipse");
          add_edge(id, name_id, "target");

          std::size_t val_id = visit_expression(*arg.value);
          add_edge(id, val_id, "value");

          return id;

        } else if constexpr (std::is_same_v<T, CallExpr>) {
          std::size_t id = next_id();
          add_node(id, "Call\\n" + std::string(arg.callee.lexem), "hexagon");

          for (std::size_t i = 0; i < arg.arguments.size(); ++i) {
            std::size_t arg_id = visit_expression(*arg.arguments[i]);
            add_edge(id, arg_id, "arg" + std::to_string(i));
          }

          return id;

        } else if constexpr (std::is_same_v<T, GroupingExpr>) {
          std::size_t id = next_id();
          add_node(id, "( )", "circle");
          std::size_t inner_id = visit_expression(*arg.expression);
          add_edge(id, inner_id);
          return id;
        }

        return 0;
      },
      expr.data);
}

void visualize_ast(std::ostream& os, const Program& program) {
  AstVisualizer visualizer(os);
  visualizer.generate_dot(program);
}

}  // namespace scy