#include "parser.h"

std::unique_ptr<ExprAST> Parser::parseIntExpr()
{
  std::cout << "Parsing int expression." << '\n';
  auto result = llvm::make_unique<IntExprAST>(std::stoi(tkStream[curIdx].val));
  ++curIdx;
  return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parseRealExpr()
{
  std::cout << "Parsing real expresion." << '\n';
  auto result = llvm::make_unique<RealExprAST>(std::stoi(tkStream[curIdx].val));
  ++curIdx;
  return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parseStringExpr()
{
  std::cout << "Parsing string expression." << '\n';
  auto result = llvm::make_unique<StringExprAST>(tkStream[curIdx].val);
  ++curIdx;
  return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parseParenExpr()
{
  std::cout << "Parsing parenthesis expression." << '\n';
  ++curIdx; // eat '('
  auto inner = parseExpression();
  if (tkStream[curIdx].tp != tok_rParenthesis) {
    return nullptr;
  }
  ++curIdx; // eat ')'
  return inner;
}

std::vector<std::unique_ptr<ExprAST>> Parser::parseBraceExpr()
{
  std::cout << "Parsing brace expression." << '\n';
  ++curIdx; // eat '{'

  std::vector<std::unique_ptr<ExprAST>> exprs;

  while (tkStream[curIdx].tp != tok_rBrace) {
    exprs.emplace_back(std::move(parseStatement()));
  }

  ++curIdx; // eat '}'

  return exprs;
}

std::unique_ptr<ExprAST> Parser::parseSqrBrktExpr()
{
  std::cout << "Parsing square bracket expression." << '\n';  
  ++curIdx; // eat '['
  auto inner = parseExpression();
  if (!inner) {
    return nullptr;
  }
  if (tkStream[curIdx].tp != tok_rSquareBracket) {
    return nullptr;
  }
  ++curIdx; // eat ']'
  return inner;
}

std::unique_ptr<ExprAST> Parser::parseIdentifierExpr()
{
  std::cout << "Parsing identifier expression." << '\n';
  auto name = tkStream[curIdx].val;
  ++curIdx; // eat identifier
  if (tkStream[curIdx].tp == tok_lSquareBracket) {
    auto offset = parseSqrBrktExpr();
    return llvm::make_unique<VariableExprAST>(name, std::move(offset));
  } else {
    return llvm::make_unique<VariableExprAST>(name);
  }
}

std::unique_ptr<ExprAST> Parser::parseDeclareExpr()
{
  std::cout << "Parsing declaration expression." << '\n';

  int varTp = tkStream[curIdx].tp;
  ++curIdx; // eat 'var'

  std::cout << tkStream[curIdx].val << std::endl;
  if (tkStream[curIdx].tp != tok_identifier) {
    return nullptr;
  }

  auto var = llvm::make_unique<VariableExprAST>(tkStream[curIdx].val);
  ++curIdx; // eat var name

  if (tkStream[curIdx].tp != tok_assignOp) {
    return nullptr;
  }
  ++curIdx; // eat '='

  auto init = parseExpression();

  if (tkStream[curIdx].tp != tok_semicolon) {
    return nullptr;
  }
  ++curIdx; // est ';'

  return llvm::make_unique<DeclareExprAST>(varTp, std::move(var), std::move(init));
}

std::unique_ptr<ExprAST> Parser::parsePrimary()
{
  std::cout << "Parsing primary expression." << '\n';
  switch (tkStream[curIdx].tp) {
    case tok_lParenthesis:
      return parseParenExpr();
    case tok_string:
      return parseStringExpr();
    case tok_integer:
      return parseIntExpr();
    case tok_real:
      return parseRealExpr();
    case tok_identifier:
      return parseIdentifierExpr();
    default:
      return nullptr;
  }
}

std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int prec, std::unique_ptr<ExprAST> LHS)
{
  while (true) {
    std::cout << "Parsing binary operator RHS expression." << '\n';
    auto& tkOp = tkStream[curIdx];
    int curTkPrec = getCurTkPrec();
    if (curTkPrec < prec) {
      return LHS;
    }
    ++curIdx; // eat op
    auto RHS = parsePrimary();
    if (!RHS) {
      return nullptr;
    }
    int nextTkPrec = getCurTkPrec();
    if (curTkPrec < nextTkPrec) {
      RHS = parseBinOpRHS(curTkPrec + 1, std::move(RHS));
      if (!RHS) {
        return nullptr;
      }
    }
    LHS = llvm::make_unique<BinaryExprAST>(tkOp.tp, std::move(LHS), std::move(RHS));
  }
}

std::unique_ptr<ExprAST> Parser::parseExpression()
{
  std::cout << "Parsing expression." << '\n';
  auto LHS = parsePrimary();
  if (!LHS) {
    return nullptr;
  }
  return parseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<ExprAST> Parser::parseStatement()
{
  std::cout << "Parsing statement.\n";
  switch(tkStream[curIdx].tp) {
    case tok_intType:
    case tok_stringType:
    case tok_doubleType:
      return parseDeclareExpr();
    case tok_identifier:
      return parseAssignExpr();
    case tok_while:
      return parseWhileExpr();
    case tok_if:
      return parseIfExpr();
    default:
      return nullptr;
  }
}

std::unique_ptr<ExprAST> Parser::parseAssignExpr()
{
  auto var = parseIdentifierExpr();

  if (tkStream[curIdx].tp != tok_assignOp) {
    return nullptr;
  }
  ++curIdx; // eat '='

  auto expr = parseExpression();

  if (tkStream[curIdx].tp != tok_semicolon) {
    return nullptr;
  }
  ++curIdx; // eat ';'

  return llvm::make_unique<BinaryExprAST>(tok_assignOp, std::move(var), std::move(expr));
}

std::unique_ptr<ExprAST> Parser::parseIfExpr()
{
  ++curIdx; // eat 'if'

  auto cond = std::move(parseParenExpr());

  auto ifBody = parseBraceExpr();

  if (tkStream[curIdx].tp != tok_else) {
    return nullptr;
  }
  ++curIdx; // eat 'else'

  auto elseBody = parseBraceExpr();
  return llvm::make_unique<IfExprAST>(std::move(cond), std::move(ifBody), std::move(elseBody));
}

std::unique_ptr<ExprAST> Parser::parseWhileExpr()
{
  ++curIdx; // eat while

  auto cond = std::move(parseParenExpr());

  auto body = parseBraceExpr();

  return llvm::make_unique<WhileExprAST>(std::move(cond), std::move(body));
}

std::unique_ptr<FunctionAST> Parser::parseFunction()
{
  // ++curIdx; // eat 'function'

  

  return nullptr;
}

std::unique_ptr<FunctionAST> Parser::parseExtern()
{
  return nullptr;
}

void Parser::parse()
{
  curIdx = 0;
  while (curIdx < tkStream.size()) {
    std::cout << tkStream[curIdx].val << std::endl;
    switch (tkStream[curIdx].tp) {
      case tok_function:
        functions.emplace_back(std::move(parseFunction()));
        break;
      case tok_extern:
        functions.emplace_back(std::move(parseExtern()));
        break;
      default:
        expressions.emplace_back(std::move(parseStatement()));
        break;
    }
  }
}

int Parser::getCurTkPrec()
{
  auto prec = precTable.find(tkStream[curIdx].tp);
  if (prec != precTable.end()) {
    return prec->second;
  } else {
    return -1;
  }
}

void Parser::print()
{
  for (const auto& expr : expressions) {
    expr->print();
    std::cout << '\n';
  }
  for (const auto& func : functions) {
    func->print();
    std::cout << '\n';
  }
}
