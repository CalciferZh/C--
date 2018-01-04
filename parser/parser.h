#ifndef __PARSER_H__
#define __PARSER_H__

#include "./AST.h"
#include "./variable.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include <utility>

class Parser {
public:
  std::vector<Token> tkStream;

  std::map<int, int> precTable = {
    {tok_assignOp, 2},
    {tok_lessOp, 10},
    {tok_greaterOp, 10},
    {tok_addOp, 20},
    {tok_subtractOp, 20},
    {tok_multiplyOp, 40},
    {tok_divideOp, 40}
  };

  std::map<std::string, std::unique_ptr<Variable>> varTable;

  llvm::LLVMContext context;

  llvm::IRBuilder<> builder;

  std::unique_ptr<llvm::Module> module;

  std::vector<std::unique_ptr<FunctionAST>> functions;

  std::vector<std::unique_ptr<ExprAST>> expressions;

  size_t curIdx;

  Parser(std::vector<Token> tkStream) : tkStream(std::move(tkStream)), builder(llvm::IRBuilder<>(context)) {}

  void parse();

  // check for parse errors (literally, nullptr in vector)
  void check();

  void print();

  // i.e. 3.1415
  std::unique_ptr<ExprAST> parseRealExpr();

  // i.e. 3
  std::unique_ptr<ExprAST> parseIntExpr();

  // i.e. "fxxk"
  std::unique_ptr<ExprAST> parseStringExpr();

  // i.e. ("expression")
  std::unique_ptr<ExprAST> parseParenExpr();

  // i.e. {"expression"}
  std::vector<std::unique_ptr<ExprAST>> parseBraceExpr();

  // i.e. ["expression"]
  std::unique_ptr<ExprAST> parseSqrBrktExpr();

  // i.e. anAwesomeVarName
  std::unique_ptr<ExprAST> parseIdentifierExpr();

  // root node of AST
  std::unique_ptr<ExprAST> parsePrimary();

  // i.e. + b * c
  std::unique_ptr<ExprAST> parseBinOpRHS(int prec, std::unique_ptr<ExprAST> LHS);

  // basic expression
  std::unique_ptr<ExprAST> parseExpression();

  // i.e. var i = 3.14
  std::unique_ptr<ExprAST> parseDeclareExpr();

  // while loop
  std::unique_ptr<ExprAST> parseWhileExpr();

  std::unique_ptr<ExprAST> parseAssignExpr();

  std::unique_ptr<ExprAST> parseIfExpr();

  std::vector<std::unique_ptr<Variable>> parseArguments();

  std::unique_ptr<PrototypeAST> parsePrototype();

  std::unique_ptr<FunctionAST> parseFunction();

  std::unique_ptr<FunctionAST> parseExtern();

  std::unique_ptr<ExprAST> parseRetExpr();

  std::unique_ptr<ExprAST> parseStatement();

  int getCurTkPrec();
};

#endif
