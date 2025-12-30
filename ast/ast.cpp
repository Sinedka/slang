#include "ast.h"
#include <string>

// === Реализация узлов AST ===

IdentifierNode::IdentifierNode(const std::string& n) : name(n) {
  type = NodeType::IDENTIFIER;
}

void IdentifierNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "Identifier: " << name << "\n";
}

LiteralNode::LiteralNode(const std::string& v) : value(v) {
  type = NodeType::LITERAL;
}

void LiteralNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "Literal: " << value << "\n";
}

BinaryOpNode::BinaryOpNode(sl_ttype o, std::unique_ptr<ExprNode> l, 
                           std::unique_ptr<ExprNode> r)
  : op(o), left(std::move(l)), right(std::move(r)) {
  type = NodeType::BINARY_OP;
}

void BinaryOpNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "BinaryOp: " << (int)op << "\n";
  left->print(indent + 2);
  right->print(indent + 2);
}

UnaryOpNode::UnaryOpNode(sl_ttype o, std::unique_ptr<ExprNode> opnd)
  : op(o), operand(std::move(opnd)) {
  type = NodeType::UNARY_OP;
}

void UnaryOpNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "UnaryOp: " << (int)op << "\n";
  operand->print(indent + 2);
}

CallExprNode::CallExprNode(const std::string& name) : func_name(name) {
  type = NodeType::CALL_EXPR;
}

void CallExprNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "Call: " << func_name << "\n";
  for (const auto& arg : args) {
    arg->print(indent + 2);
  }
}

MemberAccessNode::MemberAccessNode(std::unique_ptr<ExprNode> obj, 
                                   const std::string& mem, bool arrow)
  : object(std::move(obj)), member(mem), is_arrow(arrow) {
  type = NodeType::MEMBER_ACCESS;
}

void MemberAccessNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "MemberAccess: " 
            << (is_arrow ? "->" : ".") << member << "\n";
  object->print(indent + 2);
}

VarDeclNode::VarDeclNode(const std::string& t, const std::string& n)
  : var_type(t), name(n) {
  type = NodeType::VAR_DECL;
}

void VarDeclNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "VarDecl: " << var_type 
            << " " << name << "\n";
  if (init) init->print(indent + 2);
}

GlobalVarDeclNode::GlobalVarDeclNode(const std::string& t, const std::string& n)
  : var_type(t), name(n) {
  type = NodeType::GLOBAL_VAR_DECL;
}

void GlobalVarDeclNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "GlobalVarDecl: " << var_type 
            << " " << name << "\n";
  if (init) init->print(indent + 2);
}

CompoundStmtNode::CompoundStmtNode() { 
  type = NodeType::COMPOUND_STMT; 
}

void CompoundStmtNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "CompoundStmt {\n";
  for (const auto& stmt : statements) {
    stmt->print(indent + 2);
  }
  std::cout << std::string(indent, ' ') << "}\n";
}

ReturnStmtNode::ReturnStmtNode(std::unique_ptr<ExprNode> e) 
  : expr(std::move(e)) {
  type = NodeType::RETURN_STMT;
}

void ReturnStmtNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "Return\n";
  if (expr) expr->print(indent + 2);
}

IfStmtNode::IfStmtNode(std::unique_ptr<ExprNode> cond, 
                       std::unique_ptr<StmtNode> then_s)
  : condition(std::move(cond)), then_stmt(std::move(then_s)) {
  type = NodeType::IF_STMT;
}

void IfStmtNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "If\n";
  condition->print(indent + 2);
  then_stmt->print(indent + 2);
  if (else_stmt) else_stmt->print(indent + 2);
}

WhileStmtNode::WhileStmtNode(std::unique_ptr<ExprNode> cond, 
                             std::unique_ptr<StmtNode> b)
  : condition(std::move(cond)), body(std::move(b)) {
  type = NodeType::WHILE_STMT;
}

void WhileStmtNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "While\n";
  condition->print(indent + 2);
  body->print(indent + 2);
}

ExprStmtNode::ExprStmtNode(std::unique_ptr<ExprNode> e) 
  : expr(std::move(e)) {
  type = NodeType::EXPR_STMT;
}

void ExprStmtNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "ExprStmt\n";
  if (expr) expr->print(indent + 2);
}

FunctionDefNode::FunctionDefNode(const std::string& ret, const std::string& n, bool method)
  : return_type(ret), name(n), is_method(method) {
  type = NodeType::FUNCTION_DEF;
}

void FunctionDefNode::print(int indent) const {
  std::cout << std::string(indent, ' ') 
            << (is_method ? "Method: " : "Function: ") 
            << return_type << " " << name << "\n";
  for (const auto& param : params) {
    param->print(indent + 2);
  }
  if (body) body->print(indent + 2);
}

StructDefNode::StructDefNode(const std::string& n) : name(n) {
  type = NodeType::STRUCT_DEF;
}

void StructDefNode::print(int indent) const {
  std::cout << std::string(indent, ' ') << "StructDef: " << name << " {\n";
  
  if (!members.empty()) {
    std::cout << std::string(indent + 2, ' ') << "Members:\n";
    for (const auto& member : members) {
      member->print(indent + 4);
    }
  }
  
  if (!methods.empty()) {
    std::cout << std::string(indent + 2, ' ') << "Methods:\n";
    for (const auto& method : methods) {
      method->print(indent + 4);
    }
  }
  
  std::cout << std::string(indent, ' ') << "}\n";
}

ProgramNode::ProgramNode() { 
  type = NodeType::PROGRAM; 
}

void ProgramNode::print(int indent) const {
  std::cout << "Program\n";
  for (const auto& decl : declarations) {
    decl->print(indent + 2);
  }
}

// === Реализация парсера ===

Parser::Parser(std::string filename) {
  pfile = new spp_reader((char*)filename.c_str());
  current = nullptr;
  advance();
}

void Parser::advance() {
  current = pfile->get_token();
}

bool Parser::match(sl_ttype t) {
  return current && current->type == t;
}

bool Parser::expect(sl_ttype t, const char* msg) {
  if (!match(t)) {
    throw std::runtime_error("line: " + std::to_string(current->src_loc) + " " + msg);
  }
  advance();
  return true;
}

std::string Parser::get_identifier() {
  if (!current || current->type != SPP_NAME) {
    throw std::runtime_error("line: " + std::to_string(current->src_loc) + " Expected identifier");
  }
  std::string name(current->val.str.text, current->val.str.len);
  advance();
  return name;
}

std::string Parser::peek_identifier() {
  if (!current || current->type != SPP_NAME) {
    return "";
  }
  return std::string(current->val.str.text, current->val.str.len);
}

bool Parser::is_type_keyword(const std::string& s) {
  return s == "int" || s == "float" || s == "char" || 
         s == "void" || s == "double" || s == "long" ||
         s == "short" || s == "unsigned" || s == "signed" ||
         s == "struct" || s == "const" || s == "static";
}

std::string Parser::parse_type() {
  if (!current || current->type != SPP_NAME) {
    throw std::runtime_error("line: " + std::to_string(current->src_loc) + " Expected type");
  }
  
  std::string type_str;
  
  // Обработка модификаторов типа (const, static, unsigned, signed, long, short)
  while (match(SPP_NAME)) {
    std::string token = peek_identifier();
    
    if (token == "const" || token == "static" || 
        token == "unsigned" || token == "signed" ||
        token == "long" || token == "short") {
      if (!type_str.empty()) {
        type_str += " ";
      }
      type_str += get_identifier();
    } else {
      break;
    }
  }
  
  // Основной тип (int, float, char, void, double, struct)
  if (!match(SPP_NAME)) {
    throw std::runtime_error("line: " + std::to_string(current->src_loc) + " Expected base type");
  }
  
  std::string base_type = peek_identifier();
  
  if (!is_type_keyword(base_type)) {
    throw std::runtime_error("line: " + std::to_string(current->src_loc) + " Expected type keyword, got: " + base_type);
  }
  
  if (!type_str.empty()) {
    type_str += " ";
  }
  type_str += get_identifier();
  
  // Если это struct, добавляем имя структуры
  if (base_type == "struct" && match(SPP_NAME)) {
    type_str += " " + get_identifier();
  }
  
  // Обработка указателей
  while (match(SPP_MULT)) {
    type_str += "*";
    advance();
  }
  
  return type_str;
}

int Parser::get_precedence(sl_ttype t) {
  switch (t) {
    case SPP_MULT: case SPP_DIV: case SPP_MOD: return 3;
    case SPP_PLUS: case SPP_MINUS: return 4;
    case SPP_LSHIFT: case SPP_RSHIFT: return 5;
    case SPP_LESS: case SPP_LESS_EQ: case SPP_GREATER: case SPP_GREATER_EQ: return 6;
    case SPP_EQ_EQ: case SPP_NOT_EQ: return 7;
    case SPP_AND: return 8;
    case SPP_XOR: return 9;
    case SPP_OR: return 10;
    case SPP_AND_AND: return 11;
    case SPP_OR_OR: return 12;
    case SPP_EQ: case SPP_PLUS_EQ: case SPP_MINUS_EQ: 
    case SPP_MULT_EQ: case SPP_DIV_EQ: return 14;
    default: return -1;
  }
}

bool Parser::is_binary_op(sl_ttype t) {
  return get_precedence(t) != -1;
}

std::unique_ptr<ExprNode> Parser::parse_primary() {
  if (match(SPP_NAME)) {
    std::string name = get_identifier();
    
    // Вызов функции
    if (match(SPP_OPEN_PAREN)) {
      advance();
      auto call = std::make_unique<CallExprNode>(name);
      
      if (!match(SPP_CLOSE_PAREN)) {
        do {
          call->args.push_back(parse_expression());
        } while (match(SPP_COMMA) && (advance(), true));
      }
      
      expect(SPP_CLOSE_PAREN);
      return call;
    }
    
    return std::make_unique<IdentifierNode>(name);
  }
  
  if (match(SPP_NUMBER) || match(SPP_CHAR) || match(SPP_STRING)) {
    std::string val(current->val.str.text, current->val.str.len);
    advance();
    return std::make_unique<LiteralNode>(val);
  }
  
  if (match(SPP_OPEN_PAREN)) {
    advance();
    auto expr = parse_expression();
    expect(SPP_CLOSE_PAREN);
    return expr;
  }
  
  throw std::runtime_error("line: " + std::to_string(current->src_loc) + " Expected primary expression");
}

std::unique_ptr<ExprNode> Parser::parse_postfix() {
  auto expr = parse_primary();
  
  while (true) {
    // Доступ к членам: obj.member или obj->member
    if (match(SPP_DOT)) {
      advance();
      std::string member = get_identifier();
      expr = std::make_unique<MemberAccessNode>(std::move(expr), member, false);
    }
    else if (match(SPP_DEREF)) {
      advance();
      std::string member = get_identifier();
      expr = std::make_unique<MemberAccessNode>(std::move(expr), member, true);
    }
    // Индексация массива: arr[index]
    else if (match(SPP_OPEN_SQUARE)) {
      advance();
      auto index = parse_expression();
      expect(SPP_CLOSE_SQUARE);
      expr = std::make_unique<BinaryOpNode>(SPP_OPEN_SQUARE, std::move(expr), std::move(index));
    }
    // Постфиксный инкремент/декремент
    else if (match(SPP_PLUS_PLUS) || match(SPP_MINUS_MINUS)) {
      sl_ttype op = current->type;
      advance();
      expr = std::make_unique<UnaryOpNode>(op, std::move(expr));
    }
    else {
      break;
    }
  }
  
  return expr;
}

std::unique_ptr<ExprNode> Parser::parse_unary() {
  if (match(SPP_PLUS) || match(SPP_MINUS) || match(SPP_NOT) || 
      match(SPP_COMPL) || match(SPP_MULT) || match(SPP_AND) ||
      match(SPP_PLUS_PLUS) || match(SPP_MINUS_MINUS)) {
    sl_ttype op = current->type;
    advance();
    return std::make_unique<UnaryOpNode>(op, parse_unary());
  }
  
  return parse_postfix();
}

std::unique_ptr<ExprNode> Parser::parse_binary(int min_prec) {
  auto left = parse_unary();
  
  while (is_binary_op(current->type)) {
    int prec = get_precedence(current->type);
    if (prec > min_prec) break;
    
    sl_ttype op = current->type;
    advance();
    
    auto right = parse_binary(prec + 1);
    left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
  }
  
  return left;
}

std::unique_ptr<ExprNode> Parser::parse_expression() {
  return parse_binary(0);
}

std::unique_ptr<StmtNode> Parser::parse_statement() {
  if (!current) {
    throw std::runtime_error("line: " + std::to_string(current->src_loc) + " Unexpected end of input");
  }
  
  if (match(SPP_NAME)) {
    std::string keyword = peek_identifier();
    
    if (keyword == "return") {
      advance();
      auto expr = match(SPP_SEMICOLON) ? nullptr : parse_expression();
      expect(SPP_SEMICOLON);
      return std::make_unique<ReturnStmtNode>(std::move(expr));
    }
    
    if (keyword == "if") {
      advance();
      expect(SPP_OPEN_PAREN);
      auto cond = parse_expression();
      expect(SPP_CLOSE_PAREN);
      auto then_stmt = parse_statement();
      
      auto if_node = std::make_unique<IfStmtNode>(std::move(cond), std::move(then_stmt));
      
      if (match(SPP_NAME)) {
        std::string else_kw = peek_identifier();
        if (else_kw == "else") {
          advance();
          if_node->else_stmt = parse_statement();
        }
      }
      
      return if_node;
    }
    
    if (keyword == "while") {
      advance();
      expect(SPP_OPEN_PAREN);
      auto cond = parse_expression();
      expect(SPP_CLOSE_PAREN);
      auto body = parse_statement();
      return std::make_unique<WhileStmtNode>(std::move(cond), std::move(body));
    }
    
    // Декларация переменной
    if (is_type_keyword(keyword)) {
      std::string type = parse_type();
      std::string name = get_identifier();
      auto decl = std::make_unique<VarDeclNode>(type, name);
      
      if (match(SPP_EQ)) {
        advance();
        decl->init = parse_expression();
      }
      
      expect(SPP_SEMICOLON);
      return decl;
    }
  }
  
  // Составная инструкция
  if (match(SPP_OPEN_BRACE)) {
    return parse_compound_statement();
  }
  
  // Выражение-инструкция
  auto expr = parse_expression();
  expect(SPP_SEMICOLON);
  return std::make_unique<ExprStmtNode>(std::move(expr));
}

std::unique_ptr<CompoundStmtNode> Parser::parse_compound_statement() {
  expect(SPP_OPEN_BRACE);
  auto compound = std::make_unique<CompoundStmtNode>();
  
  while (!match(SPP_CLOSE_BRACE) && !match(SPP_EOF)) {
    compound->statements.push_back(parse_statement());
  }
  
  expect(SPP_CLOSE_BRACE);
  return compound;
}

std::unique_ptr<StructDefNode> Parser::parse_struct() {
  expect(SPP_NAME); // "struct"
  std::string name = get_identifier();
  
  auto struct_def = std::make_unique<StructDefNode>(name);
  
  expect(SPP_OPEN_BRACE);
  
  while (!match(SPP_CLOSE_BRACE) && !match(SPP_EOF)) {
    std::string first_token = peek_identifier();
    
    if (!is_type_keyword(first_token)) {
      throw std::runtime_error("line: " + std::to_string(current->src_loc) + " Expected type keyword in struct");
    }
    
    std::string member_type = parse_type();
    std::string member_name = get_identifier();
    
    // Это метод?
    if (match(SPP_OPEN_PAREN)) {
      // Парсим как метод
      auto method = std::make_unique<FunctionDefNode>(member_type, member_name, true);
      
      advance(); // пропускаем (
      
      if (!match(SPP_CLOSE_PAREN)) {
        do {
          std::string param_type = parse_type();
          std::string param_name = get_identifier();
          method->params.push_back(std::make_unique<VarDeclNode>(param_type, param_name));
        } while (match(SPP_COMMA) && (advance(), true));
      }
      
      expect(SPP_CLOSE_PAREN);
      method->body = parse_compound_statement();
      
      struct_def->methods.push_back(std::move(method));
    }
    else {
      // Это обычное поле
      struct_def->members.push_back(std::make_unique<VarDeclNode>(member_type, member_name));
      expect(SPP_SEMICOLON);
    }
  }
  
  expect(SPP_CLOSE_BRACE);
  expect(SPP_SEMICOLON);
  
  return struct_def;
}

std::unique_ptr<FunctionDefNode> Parser::parse_function(bool is_method) {
  std::string ret_type = parse_type();
  std::string name = get_identifier();
  auto func = std::make_unique<FunctionDefNode>(ret_type, name, is_method);
  
  expect(SPP_OPEN_PAREN);
  
  if (!match(SPP_CLOSE_PAREN)) {
    do {
      std::string param_type = parse_type();
      std::string param_name = get_identifier();
      func->params.push_back(std::make_unique<VarDeclNode>(param_type, param_name));
    } while (match(SPP_COMMA) && (advance(), true));
  }
  
  expect(SPP_CLOSE_PAREN);
  func->body = parse_compound_statement();
  
  return func;
}

std::unique_ptr<GlobalVarDeclNode> Parser::parse_global_var() {
  expect(SPP_NAME); // "global"
  
  std::string var_type = parse_type();
  std::string name = get_identifier();
  auto global_var = std::make_unique<GlobalVarDeclNode>(var_type, name);
  
  if (match(SPP_EQ)) {
    advance();
    global_var->init = parse_expression();
  }
  
  expect(SPP_SEMICOLON);
  
  return global_var;
}

std::unique_ptr<ASTNode> Parser::parse_declaration() {
  if (!current || !match(SPP_NAME)) {
    throw std::runtime_error("line: " + std::to_string(current->src_loc) + " Expected declaration");
  }
  
  std::string keyword(current->val.str.text, current->val.str.len);
  
  // Глобальная переменная
  if (keyword == "global") {
    return parse_global_var();
  }
  
  // Структура
  if (keyword == "struct") {
    return parse_struct();
  }
  
  // Функция
  return parse_function();
}

std::unique_ptr<ProgramNode> Parser::parse() {
  auto program = std::make_unique<ProgramNode>();
  
  while (!match(SPP_EOF)) {
    program->declarations.push_back(parse_declaration());
  }
  
  return program;
}
