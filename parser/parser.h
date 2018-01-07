#ifndef __PARSER_H__
#define __PARSER_H__

#include "./AST.h"
#include "./variable.h"
#include "./function.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "./exception.h"
#include <utility>

class Parser {
public:
  std::vector<Token> tkStream;

  std::map<int, int> precTable = {
    {tok_assignOp, 1},
    {tok_lessOp, 100},
    {tok_greaterOp, 100},
    {tok_logicOrOp, 1},
    {tok_logicAndOp, 1},
    {tok_euqalOp, 100},
    {tok_nEqualOp, 100},
    {tok_addOp, 200},
    {tok_subtractOp, 200},
    {tok_multiplyOp, 400},
    {tok_divideOp, 400}
  };

  std::map<std::string, std::unique_ptr<Variable>> varTable;

  std::map<std::string, std::unique_ptr<Function>> funcTable;

  llvm::LLVMContext context;

  llvm::IRBuilder<> builder;

  std::unique_ptr<llvm::Module> module;

  std::vector<std::unique_ptr<ExprAST>> expressions;

  std::vector<std::unique_ptr<FunctionAST>> functions; // this holds all function definition AST

  size_t curIdx;

  Parser(std::vector<Token> tkStream) : tkStream(std::move(tkStream)), builder(llvm::IRBuilder<>(context)) {}

  void parse();

  // check for parse errors (literally, nullptr in vector)
  void check();

  void print();

  std::unique_ptr<ExprAST> parseRealExpr();

  std::unique_ptr<ExprAST> parseIntExpr();

  std::unique_ptr<ExprAST> parseStringExpr();

  std::unique_ptr<ExprAST> parseCharExpr();

  std::unique_ptr<ExprAST> parseParenExpr();

  std::vector<std::unique_ptr<ExprAST>> parseBraceExpr();

  std::unique_ptr<ExprAST> parseSqrBrktExpr();

  std::unique_ptr<ExprAST> parseIdentifierExpr();

  std::unique_ptr<ExprAST> parsePrimary();

  std::unique_ptr<ExprAST> parseBinOpRHS(int prec, std::unique_ptr<ExprAST> LHS);

  std::unique_ptr<ExprAST> parseExpression();

  std::unique_ptr<ExprAST> parseDeclareExpr();

  std::unique_ptr<ExprAST> parseWhileExpr();

  std::unique_ptr<ExprAST> parseAssignExpr();

  std::unique_ptr<ExprAST> parseIfExpr();

  std::unique_ptr<ExprAST> parseBreakExpr();

  std::unique_ptr<ExprAST> parseSingleExpr(); // parse expressions as a single statement, i.e. a = max(b,c);

  std::unique_ptr<ExprAST> parseInitListExpr();

  std::vector<std::unique_ptr<ExprAST>> parseParams();

  std::vector<std::unique_ptr<Variable>> parseArguments();

  std::unique_ptr<PrototypeAST> parsePrototype();

  std::unique_ptr<FunctionAST> parseFunction();

  std::unique_ptr<FunctionAST> parseExtern();

  std::unique_ptr<ExprAST> parseRetExpr();

  std::unique_ptr<ExprAST> parseStatement();

  int getCurTkPrec();
};

#endif
