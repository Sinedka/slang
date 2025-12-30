#ifndef C_PARSER_H
#define C_PARSER_H

#include <memory>
#include <vector>
#include <string>
#include "spplib.h"


// === AST узлы ===
enum class NodeType {
  PROGRAM,
  FUNCTION_DEF,
  VAR_DECL,
  PARAM_DECL,
  COMPOUND_STMT,
  RETURN_STMT,
  IF_STMT,
  WHILE_STMT,
  FOR_STMT,
  EXPR_STMT,
  BINARY_OP,
  UNARY_OP,
  CALL_EXPR,
  LITERAL,
  IDENTIFIER,
  CAST_EXPR,
  ASSIGN_EXPR,
  STRUCT_DEF,
  MEMBER_ACCESS,
  GLOBAL_VAR_DECL
};

struct ASTNode {
  NodeType type;
  virtual ~ASTNode() = default;
  virtual void print(int indent = 0) const = 0;
};

struct ExprNode : public ASTNode {};

struct StmtNode : public ASTNode {};

struct IdentifierNode : public ExprNode {
  std::string name;
  IdentifierNode(const std::string& n);
  void print(int indent = 0) const override;
};

struct LiteralNode : public ExprNode {
  std::string value;
  LiteralNode(const std::string& v);
  void print(int indent = 0) const override;
};

struct BinaryOpNode : public ExprNode {
  sl_ttype op;
  std::unique_ptr<ExprNode> left;
  std::unique_ptr<ExprNode> right;
  BinaryOpNode(sl_ttype o, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r);
  void print(int indent = 0) const override;
};

struct UnaryOpNode : public ExprNode {
  sl_ttype op;
  std::unique_ptr<ExprNode> operand;
  UnaryOpNode(sl_ttype o, std::unique_ptr<ExprNode> opnd);
  void print(int indent = 0) const override;
};

struct CallExprNode : public ExprNode {
  std::string func_name;
  std::vector<std::unique_ptr<ExprNode>> args;
  CallExprNode(const std::string& name);
  void print(int indent = 0) const override;
};

struct MemberAccessNode : public ExprNode {
  std::unique_ptr<ExprNode> object;
  std::string member;
  bool is_arrow; // true для ->, false для .
  MemberAccessNode(std::unique_ptr<ExprNode> obj, const std::string& mem, bool arrow);
  void print(int indent = 0) const override;
};

struct VarDeclNode : public StmtNode {
  std::string var_type;
  std::string name;
  std::unique_ptr<ExprNode> init;
  VarDeclNode(const std::string& t, const std::string& n);
  void print(int indent = 0) const override;
};

struct GlobalVarDeclNode : public ASTNode {
  std::string var_type;
  std::string name;
  std::unique_ptr<ExprNode> init;
  GlobalVarDeclNode(const std::string& t, const std::string& n);
  void print(int indent = 0) const override;
};

struct CompoundStmtNode : public StmtNode {
  std::vector<std::unique_ptr<StmtNode>> statements;
  CompoundStmtNode();
  void print(int indent = 0) const override;
};

struct ReturnStmtNode : public StmtNode {
  std::unique_ptr<ExprNode> expr;
  ReturnStmtNode(std::unique_ptr<ExprNode> e);
  void print(int indent = 0) const override;
};

struct IfStmtNode : public StmtNode {
  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<StmtNode> then_stmt;
  std::unique_ptr<StmtNode> else_stmt;
  IfStmtNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> then_s);
  void print(int indent = 0) const override;
};

struct WhileStmtNode : public StmtNode {
  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<StmtNode> body;
  WhileStmtNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> b);
  void print(int indent = 0) const override;
};

struct ExprStmtNode : public StmtNode {
  std::unique_ptr<ExprNode> expr;
  ExprStmtNode(std::unique_ptr<ExprNode> e);
  void print(int indent = 0) const override;
};

struct FunctionDefNode : public ASTNode {
  std::string return_type;
  std::string name;
  std::vector<std::unique_ptr<VarDeclNode>> params;
  std::unique_ptr<CompoundStmtNode> body;
  bool is_method; // true если это метод структуры
  FunctionDefNode(const std::string& ret, const std::string& n, bool method = false);
  void print(int indent = 0) const override;
};

struct StructDefNode : public ASTNode {
  std::string name;
  std::vector<std::unique_ptr<VarDeclNode>> members;
  std::vector<std::unique_ptr<FunctionDefNode>> methods;
  StructDefNode(const std::string& n);
  void print(int indent = 0) const override;
};

struct ProgramNode : public ASTNode {
  std::vector<std::unique_ptr<ASTNode>> declarations;
  ProgramNode();
  void print(int indent = 0) const override;
};

// === Парсер ===
class Parser {
private:
  spp_reader* pfile;
  spp_token* current;
  
  void advance();
  bool match(sl_ttype t);
  bool expect(sl_ttype t, const char* msg = "Unexpected token");
  std::string get_identifier();
  std::string peek_identifier();
  
  int get_precedence(sl_ttype t);
  bool is_binary_op(sl_ttype t);
  bool is_type_keyword(const std::string& s);
  std::string parse_type();
  
  std::unique_ptr<ExprNode> parse_primary();
  std::unique_ptr<ExprNode> parse_postfix();
  std::unique_ptr<ExprNode> parse_unary();
  std::unique_ptr<ExprNode> parse_binary(int min_prec);
  std::unique_ptr<ExprNode> parse_expression();
  
  std::unique_ptr<StmtNode> parse_statement();
  std::unique_ptr<CompoundStmtNode> parse_compound_statement();
  std::unique_ptr<StructDefNode> parse_struct();
  std::unique_ptr<FunctionDefNode> parse_function(bool is_method = false);
  std::unique_ptr<GlobalVarDeclNode> parse_global_var();
  std::unique_ptr<ASTNode> parse_declaration();

public:
  Parser(std::string filename);
  std::unique_ptr<ProgramNode> parse();
};

#endif // C_PARSER_H
