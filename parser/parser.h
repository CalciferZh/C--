#ifndef __PARSER_H__
#define __PARSER_H__

#include "AST.h"
#include "../lexer/lexer.h"

class Parser {
public:
  std::vector<Token> tkStream;

  static std::map<int, int> precTable;

  std::map<std::string, llvm::AllocaInst*> varTable;

  llvm::LLVMContext context;

  llvm::IRBuilder<> builder(context);

  std::unique_ptr<Module> module;

  std::vector<std::unique<FunctionAST>> functions;

  std::vector<std::unique<ExprAST>> expressions;

  size_type curIdx;

  Parser(std::vector<Token> tkStream) : tkStream(std::move(tkStream)) {}

  void parse();

  // check for parse errors
  void check();

  // i.e. 3.1415
  std::unique_ptr<ExprAST> parseRealExpr();

  // i.e. 3
  std::unique_ptr<ExprAST> parseIntExpr();

  // i.e. "fxxk"
  std::unique_ptr<ExprAST> parseStringExpr();

  // i.e. ("expression")
  std::unique_ptr<ExprAST> parseParenExpr();

  // i.e. {"expression"}
  std::unique_ptr<ExprAST> parseBraceExpr();

  // i.e. ["expression"]
  std::unique_ptr<ExprAST> parseSqrBrktExpr();

  // i.e. anAwesomeVarName
  std::unique_ptr<ExprAST> parseIdentifierExpr();

  // root node of AST
  std::unique_ptr<ExprAST> parsePrimary();

  // i.e. + b * c
  std::unique_ptr<ExprAST> parseBinOpRHS(int prec, std::unique_ptr<ExprAST> LHS);

  // 
  std::unique_ptr<ExprAST> parseExpression();

  // i.e. var i = 3.14
  std::unique_ptr<ExprAST> parseDeclareExpr();

  int getCurTkPrec();
}

#endif
