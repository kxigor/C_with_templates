#include <gtest/gtest.h>

#include <include/ast.hpp>
#include <include/lexer.hpp>
#include <include/parser.hpp>
#include <sstream>

namespace scy::test {

class ParserTest : public ::testing::Test {
 protected:
  Program parse(StringViewT source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));
    return parser.parse();
  }
};

TEST_F(ParserTest, SimpleFunction) {
  auto program = parse("int main() { return 0; }");

  ASSERT_EQ(program.declarations.size(), 1);

  auto* func = std::get_if<FunctionDecl>(&program.declarations[0]->data);
  ASSERT_NE(func, nullptr);
  EXPECT_EQ(func->name.lexem, "main");
  EXPECT_EQ(func->return_type.lexem, "int");
  EXPECT_TRUE(func->params.empty());
  ASSERT_EQ(func->body.size(), 1);
}

TEST_F(ParserTest, FunctionWithParameters) {
  auto program = parse("int add(int a, int b) { return a + b; }");

  auto* func = std::get_if<FunctionDecl>(&program.declarations[0]->data);
  ASSERT_NE(func, nullptr);
  ASSERT_EQ(func->params.size(), 2);
  EXPECT_EQ(func->params[0].name.lexem, "a");
  EXPECT_EQ(func->params[1].name.lexem, "b");
}

TEST_F(ParserTest, GlobalVariable) {
  auto program = parse("int x = 42;");

  ASSERT_EQ(program.declarations.size(), 1);
  auto* var = std::get_if<GlobalVarDecl>(&program.declarations[0]->data);
  ASSERT_NE(var, nullptr);
  EXPECT_EQ(var->name.lexem, "x");
  EXPECT_TRUE(var->initializer.has_value());
}

TEST_F(ParserTest, IfStatement) {
  auto program = parse("void test() { if (x == 1) { print x; } }");

  auto* func = std::get_if<FunctionDecl>(&program.declarations[0]->data);
  ASSERT_EQ(func->body.size(), 1);

  auto* if_stmt = std::get_if<IfStmt>(&func->body[0]->data);
  ASSERT_NE(if_stmt, nullptr);
  EXPECT_FALSE(if_stmt->else_branch.has_value());
}

TEST_F(ParserTest, IfElseStatement) {
  auto program =
      parse("void test() { if (x > 0) { return 1; } else { return 0; } }");

  auto* func = std::get_if<FunctionDecl>(&program.declarations[0]->data);
  auto* if_stmt = std::get_if<IfStmt>(&func->body[0]->data);
  ASSERT_NE(if_stmt, nullptr);
  EXPECT_TRUE(if_stmt->else_branch.has_value());
}

TEST_F(ParserTest, BinaryExpressions) {
  auto program = parse("int main() { return 1 + 2 - 3; }");

  auto* func = std::get_if<FunctionDecl>(&program.declarations[0]->data);
  auto* ret = std::get_if<ReturnStmt>(&func->body[0]->data);
  ASSERT_NE(ret, nullptr);
  EXPECT_TRUE(ret->value.has_value());

  auto* binary = std::get_if<BinaryExpr>(&(*ret->value)->data);
  ASSERT_NE(binary, nullptr);
  EXPECT_EQ(binary->op.lexem, "-");
}

TEST_F(ParserTest, UnaryExpression) {
  auto program = parse("int main() { return !0; }");

  auto* func = std::get_if<FunctionDecl>(&program.declarations[0]->data);
  auto* ret = std::get_if<ReturnStmt>(&func->body[0]->data);

  auto* unary = std::get_if<UnaryExpr>(&(*ret->value)->data);
  ASSERT_NE(unary, nullptr);
  EXPECT_EQ(unary->op.lexem, "!");
}

TEST_F(ParserTest, FunctionCall) {
  auto program = parse("int main() { return foo(1, 2); }");

  auto* func = std::get_if<FunctionDecl>(&program.declarations[0]->data);
  auto* ret = std::get_if<ReturnStmt>(&func->body[0]->data);

  auto* call = std::get_if<CallExpr>(&(*ret->value)->data);
  ASSERT_NE(call, nullptr);
  EXPECT_EQ(call->callee.lexem, "foo");
  EXPECT_EQ(call->arguments.size(), 2);
}

TEST_F(ParserTest, Assignment) {
  auto program = parse("void main() { int x; x = 10; }");

  auto* func = std::get_if<FunctionDecl>(&program.declarations[0]->data);
  ASSERT_EQ(func->body.size(), 2);

  auto* expr_stmt = std::get_if<ExpressionStmt>(&func->body[1]->data);
  auto* assign = std::get_if<AssignExpr>(&expr_stmt->expression->data);
  ASSERT_NE(assign, nullptr);
  EXPECT_EQ(assign->name.lexem, "x");
}

TEST_F(ParserTest, LocalVariableDeclaration) {
  auto program = parse("void main() { int x = 5; }");

  auto* func = std::get_if<FunctionDecl>(&program.declarations[0]->data);
  auto* var_decl = std::get_if<VarDeclStmt>(&func->body[0]->data);
  ASSERT_NE(var_decl, nullptr);
  EXPECT_EQ(var_decl->name.lexem, "x");
  EXPECT_TRUE(var_decl->initializer.has_value());
}

TEST_F(ParserTest, PrintAst) {
  auto program = parse("int main() { print 42; return 0; }");

  std::ostringstream oss;
  print_ast(oss, program);

  std::string output = oss.str();
  EXPECT_NE(output.find("Program"), std::string::npos);
  EXPECT_NE(output.find("FunctionDecl"), std::string::npos);
  EXPECT_NE(output.find("PrintStmt"), std::string::npos);
  EXPECT_NE(output.find("ReturnStmt"), std::string::npos);
}

TEST_F(ParserTest, ComplexProgram) {
  StringViewT code = R"(
    int global = 100;
    
    int add(int a, int b) {
      return a + b;
    }
    
    int main() {
      int x = 10;
      int y = 20;
      if (x < y) {
        print add(x, y);
      } else {
        print 0;
      }
      return 0;
    }
  )";

  auto program = parse(code);

  ASSERT_EQ(program.declarations.size(), 3);

  // Global variable
  auto* global = std::get_if<GlobalVarDecl>(&program.declarations[0]->data);
  ASSERT_NE(global, nullptr);
  EXPECT_EQ(global->name.lexem, "global");

  // add function
  auto* add_func = std::get_if<FunctionDecl>(&program.declarations[1]->data);
  ASSERT_NE(add_func, nullptr);
  EXPECT_EQ(add_func->name.lexem, "add");

  // main function
  auto* main_func = std::get_if<FunctionDecl>(&program.declarations[2]->data);
  ASSERT_NE(main_func, nullptr);
  EXPECT_EQ(main_func->name.lexem, "main");
}

}  // namespace scy::test