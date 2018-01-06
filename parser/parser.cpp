#include "parser.h"

std::unique_ptr<ExprAST> Parser::parseIntExpr() {
  // std::cout << "Parsing int expression." << '\n';
  auto result = llvm::make_unique<IntExprAST>(std::stoi(tkStream[curIdx].val));
  ++curIdx;
  return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parseRealExpr() {
  // std::cout << "Parsing real expresion." << '\n';
  auto result = llvm::make_unique<RealExprAST>(std::stoi(tkStream[curIdx].val));
  ++curIdx;
  return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parseStringExpr() {
  // std::cout << "Parsing string expression." << '\n';
  auto result = llvm::make_unique<StringExprAST>(tkStream[curIdx].val);
  ++curIdx;
  return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parseParenExpr() {
  // std::cout << "Parsing parenthesis expression." << '\n';
  ++curIdx; // eat '('
  auto inner = parseExpression();
  if (tkStream[curIdx].tp != tok_rParenthesis) {
    throw ParseException("')'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // eat ')'
  return inner;
}

std::vector<std::unique_ptr<ExprAST>> Parser::parseBraceExpr() {
  ++curIdx; // eat '{'
  std::vector<std::unique_ptr<ExprAST>> exprs;
  while (tkStream[curIdx].tp != tok_rBrace) {
    exprs.emplace_back(std::move(parseStatement()));
  }
  if (tkStream[curIdx].tp != tok_rBrace) {
    throw ParseException("'}'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // eat '}'
  return exprs;
}

std::vector<std::unique_ptr<ExprAST>> Parser::parseParams() {
  ++curIdx; // eat '('
  std::vector<std::unique_ptr<ExprAST>> params;
  while (tkStream[curIdx].tp != tok_rParenthesis) {
    params.emplace_back(std::move(parseExpression()));
    if (tkStream[curIdx].tp != tok_comma) {
      break;
    }
    ++curIdx; // eat ','
  }
  if (tkStream[curIdx].tp != tok_rParenthesis) {
    throw ParseException("')'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // eat ')'
  return params;
}

std::unique_ptr<ExprAST> Parser::parseSqrBrktExpr() {
  // std::cout << "Parsing square bracket expression." << '\n';  
  ++curIdx; // eat '['
  std::unique_ptr<ExprAST> inner = nullptr;
  if (tkStream[curIdx].tp != tok_rSquareBracket) {
    inner = parseExpression();
  }
  if (tkStream[curIdx].tp != tok_rSquareBracket) {
    throw ParseException("']'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // eat ']'
  return inner;
}

std::unique_ptr<ExprAST> Parser::parseIdentifierExpr() {
  // std::cout << "Parsing identifier expression." << '\n';
  auto name = tkStream[curIdx].val;
  ++curIdx; // eat identifier
  if (tkStream[curIdx].tp == tok_lSquareBracket) {
    auto offset = parseSqrBrktExpr();
    return llvm::make_unique<VariableExprAST>(name, std::move(offset));
  } else {
    if (tkStream[curIdx].tp == tok_lParenthesis) {
      auto params = parseParams();
      return llvm::make_unique<CallExprAST>(name, std::move(params));
    } else {
      return llvm::make_unique<VariableExprAST>(name);
    }
  }
}

std::unique_ptr<ExprAST> Parser::parseDeclareExpr() {
  // std::cout << "Parsing declaration expression." << '\n';
  int varTp = tkStream[curIdx].tp;
  ++curIdx; // eat 'int double char or string'
  if (tkStream[curIdx].tp != tok_identifier) {
    throw ParseException("identifier", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  auto name = tkStream[curIdx].val;
  ++curIdx; // eat var name
  int size = 0;
  if (tkStream[curIdx].tp == tok_lSquareBracket) {
    ++curIdx; // eat '['
    if (tkStream[curIdx].tp != tok_integer) {
      throw ParseException("const integer expression", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
    }
    size = std::stoi(tkStream[curIdx].val);
    ++curIdx; // eat integer
    if (tkStream[curIdx].tp != tok_rSquareBracket) {
      throw ParseException("']'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
    }
    ++curIdx; // eat ']'
  }
  std::unique_ptr<ExprAST> init = nullptr;
  if (tkStream[curIdx].tp == tok_assignOp) {
    ++curIdx; // eat '='
    init = parseExpression();
  }
  if (tkStream[curIdx].tp != tok_semicolon) {
    throw ParseException("';'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // est ';'
  return llvm::make_unique<DeclareExprAST>(varTp, std::move(name), size, std::move(init));
}

std::unique_ptr<ExprAST> Parser::parsePrimary() {
  // std::cout << "Parsing primary expression." << '\n';
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
      throw ParseException("primary expression", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
}

std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int prec, std::unique_ptr<ExprAST> LHS) {
  while (true) {
    // std::cout << "Parsing binary operator RHS expression." << '\n';
    auto& tkOp = tkStream[curIdx];
    int curTkPrec = getCurTkPrec();
    if (curTkPrec < prec) {
      return LHS;
    }
    ++curIdx; // eat op
    auto RHS = parsePrimary();
    if (!RHS) {
      throw ParseException("valid expression as RHS", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
    }
    int nextTkPrec = getCurTkPrec();
    if (curTkPrec < nextTkPrec) {
      RHS = parseBinOpRHS(curTkPrec + 1, std::move(RHS));
      if (!RHS) {
        throw ParseException("valid expression as RHS", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
      }
    }
    LHS = llvm::make_unique<BinaryExprAST>(tkOp.tp, std::move(LHS), std::move(RHS));
  }
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
  // std::cout << "Parsing expression." << '\n';
  auto LHS = parsePrimary();
  if (!LHS) {
    throw ParseException("valid expression as LHS", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  return parseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<ExprAST> Parser::parseSingleExpr() {
  auto result = parseExpression();
  if (tkStream[curIdx].tp != tok_semicolon) {
    throw ParseException("';'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // eat ';'
  return result;
}

std::unique_ptr<ExprAST> Parser::parseBreakExpr() {
  ++curIdx; // eat break
  if (tkStream[curIdx].tp != tok_semicolon) {
    throw ParseException("';'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // eat ';'
  return llvm::make_unique<BreakExprAST>();
}

std::unique_ptr<ExprAST> Parser::parseStatement() {
  // std::cout << "Parsing statement.\n";
  switch(tkStream[curIdx].tp) {
    case tok_intType:
    case tok_stringType:
    case tok_doubleType:
    case tok_charType:
      return parseDeclareExpr();
    case tok_identifier:
      return parseSingleExpr();
    case tok_while:
      return parseWhileExpr();
    case tok_if:
      return parseIfExpr();
    case tok_return:
      return parseRetExpr();
    case tok_break:
      return parseBreakExpr();
    default:
      ++curIdx;
      throw ParseException("valid statement", tkStream[curIdx-1].val.c_str(), tkStream[curIdx-1].lineNo);
  }
}

std::unique_ptr<ExprAST> Parser::parseAssignExpr() {
  auto var = parseIdentifierExpr();
  if (tkStream[curIdx].tp != tok_assignOp) {
    throw ParseException("'='", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // eat '='
  auto expr = parseExpression();
  if (tkStream[curIdx].tp != tok_semicolon) {
    throw ParseException("';'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // eat ';'
  return llvm::make_unique<BinaryExprAST>(tok_assignOp, std::move(var), std::move(expr));
}

std::unique_ptr<ExprAST> Parser::parseIfExpr() {
  ++curIdx; // eat 'if'
  if (tkStream[curIdx].tp != tok_lParenthesis) {
    throw ParseException("(expression) as condition", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  auto cond = std::move(parseParenExpr());
  if (tkStream[curIdx].tp != tok_lBrace) {
    throw ParseException("{expressions} as if-body", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  auto ifBody = parseBraceExpr();
  std::vector<std::unique_ptr<ExprAST>> elseBody;
  if (tkStream[curIdx].tp == tok_else) {
    ++curIdx; // eat 'else'
    if (tkStream[curIdx].tp != tok_lBrace) {
      throw ParseException("{expressions} as else-body", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
    }
    elseBody = std::move(parseBraceExpr());
  }
  return llvm::make_unique<IfExprAST>(std::move(cond), std::move(ifBody), std::move(elseBody));
}

std::unique_ptr<ExprAST> Parser::parseWhileExpr() {
  ++curIdx; // eat while
  if (tkStream[curIdx].tp != tok_lParenthesis) {
    throw ParseException("(expression) as condition", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  auto cond = std::move(parseParenExpr());
  if (tkStream[curIdx].tp != tok_lBrace) {
    throw ParseException("{expressions} as body", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  auto body = parseBraceExpr();
  return llvm::make_unique<WhileExprAST>(std::move(cond), std::move(body));
}

std::unique_ptr<ExprAST> Parser::parseRetExpr() {
  ++curIdx; // eat 'return'
  auto expr = std::move(parseExpression());
  if (tkStream[curIdx].tp != tok_semicolon) {
    throw ParseException("';'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // eat ';'

  return llvm::make_unique<ReturnExprAST>(std::move(expr));
}

std::vector<std::unique_ptr<Variable>> Parser::parseArguments() {
  ++curIdx; // eat '('
  std::vector<std::unique_ptr<Variable>> args;
  while (tkStream[curIdx].tp != tok_rParenthesis) {
    args.emplace_back(llvm::make_unique<Variable>(tkStream[curIdx+1].val, tkStream[curIdx].tp));
    curIdx += 2;
    if (tkStream[curIdx].tp != tok_comma) {
      break;
    }
    ++curIdx; // eat ','
  }
  if (tkStream[curIdx].tp != tok_rParenthesis) {
    throw ParseException("')'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // eat ')'
  return args;
}

std::unique_ptr<PrototypeAST> Parser::parsePrototype() {
  int retType = tkStream[curIdx].tp;
  ++curIdx; // eat return type
  if (tkStream[curIdx].tp != tok_identifier) {
    throw ParseException("identifier", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  auto name = tkStream[curIdx].val;
  ++curIdx; // eat identifier
  if (tkStream[curIdx].tp != tok_lParenthesis) {
    throw ParseException("(args) as arguments", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  auto args = parseArguments();
  return llvm::make_unique<PrototypeAST>(std::move(name), std::move(args), retType);
}

std::unique_ptr<FunctionAST> Parser::parseFunction() {
  auto proto = parsePrototype();
  auto body = parseBraceExpr();
  return llvm::make_unique<FunctionAST>(std::move(proto), std::move(body));
}

std::unique_ptr<FunctionAST> Parser::parseExtern() {
  ++curIdx; // eat 'extern'
  auto proto = parsePrototype();
  if (tkStream[curIdx].tp != tok_semicolon) {
    throw ParseException("';'", tkStream[curIdx].val.c_str(), tkStream[curIdx].lineNo);
  }
  ++curIdx; // eat ';'
  std::vector<std::unique_ptr<ExprAST>> body;
  return llvm::make_unique<FunctionAST>(std::move(proto), std::move(body));
}

void Parser::parse() {
  curIdx = 0;
  while (curIdx < tkStream.size()) {
    try {
      switch (tkStream[curIdx].tp) {
      case tok_extern:
        functions.emplace_back(std::move(parseExtern()));
        break;
      case tok_intType:
      case tok_doubleType:
      case tok_stringType:
      case tok_voidType:
      case tok_charType:
        if (tkStream[curIdx + 2].tp == tok_lParenthesis) {
          functions.emplace_back(std::move(parseFunction()));
        }
        else {
          expressions.emplace_back(std::move(parseStatement()));
        }
        break;
      default:
        expressions.emplace_back(std::move(parseStatement()));
        break;
      }
    } catch (ParseException &e) {
      e.print();
    }
  }
}

int Parser::getCurTkPrec() {
  auto prec = precTable.find(tkStream[curIdx].tp);
  if (prec != precTable.end()) {
    return prec->second;
  } else {
    return -1;
  }
}

void Parser::print() {
  std::cout << "========================= functions ==============================\n";
  for (const auto& func : functions) {
    func->print();
    std::cout << '\n';
  }
}
