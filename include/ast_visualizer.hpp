#pragma once

#include <ostream>
#include <string>

#include "ast.hpp"

namespace scy {

class AstVisualizer {
 public:
  explicit AstVisualizer(std::ostream& os) : os_(os) {}

  void generate_dot(const Program& program);

 private:
  std::size_t next_id();

  std::size_t visit_program(const Program& program);
  std::size_t visit_declaration(const Declaration& decl);
  std::size_t visit_statement(const Statement& stmt);
  std::size_t visit_expression(const Expression& expr);

  void add_node(std::size_t id, const std::string& label,
                const std::string& shape = "box");
  void add_edge(std::size_t from, std::size_t to,
                const std::string& label = "");

  std::ostream& os_;
  std::size_t id_counter_{0};
};

void visualize_ast(std::ostream& os, const Program& program);

}  // namespace scy
