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
  if (!inner) {
    return nullptr;
  }
  if (tkStream[curIdx].tp != tok_rParenthesis) {
    return nullptr;
  }
  ++curIdx; // eat ')'
  return inner;
}

std::unique_ptr<ExprAST> Parser::parseBraceExpr()
{
  std::cout << "Parsing brace expression." << '\n';
  ++curIdx; // eat '{'
  auto inner = parseExpression();
  if (!inner) {
    return nullptr;
  }
  if (tkStream[curIdx].tp != tok_rBrace) {
    return nullptr;
  }
  ++curIdx; // eat '}'
  return inner;
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
  auto result = llvm::make_unique<VariableExprAST>(tkStream[curIdx].val);
  ++curIdx;
  return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parseDeclareExpr()
{
  std::cout << "Parsing declaration expression." << '\n';
  ++curIdx; // eat 'var'
  if (tkStream[curIdx].tp != tok_identifier) {
    return nullptr;
  }
  auto& varTk = tkStream[curIdx];
  ++curIdx; // eat var name
  if (tkStream[curIdx].tp != tok_assignOp) {
    return nullptr;
  }
  ++curIdx; // eat '='
  auto init = parseExpression();
  ++curIdx; // est ';'

  return llvm::make_unique<DeclareExprAST>(varTk.val, std::move(init));
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
    default:
      return nullptr;
  }
}

std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int prec, std::unique_ptr<ExprAST> LHS)
{
  std::cout << "Parsing binary operator RHS expression." << '\n';
  while (true) {
    int curTkPrec = getCurTkPrec();
    if (curTkPrec < prec) {
      return LHS;
    }
    auto& tkOp = tkStream[curIdx];
    ++curIdx;
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

void Parser::parse()
{
  curIdx = 0;
  while (curIdx < tkStream.size()) {
    switch (tkStream[curIdx].tp) {
      case tok_var:
        expressions.emplace_back(std::move(parseDeclareExpr()));
        break;
      default:
        std::cout << "Unexpected symbol type.";
        break;
    }
  }
}

int Parser::getCurTkPrec()
{
  auto prec = precTable.find(tkStream[curIdx].tp);
  if(prec != precTable.end()) {
    return prec->first;
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
