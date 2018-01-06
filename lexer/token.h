#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <utility>
#include <string>

enum {
  // keywords
  tok_var = -1, // maybe not used
  tok_if = -2,
  tok_else = -3,
  tok_while = -4,

  // symbols & operators
  tok_assignOp = -5,
  tok_semicolon = -6,
  tok_lParenthesis = -7,
  tok_rParenthesis = -8,
  tok_lBrace = -9,
  tok_rBrace = -10,
  tok_addOp = -11,
  tok_subtractOp = -12,
  tok_multiplyOp = -13,
  tok_divideOp = -14,

  // supported constant expression
  tok_string = -15,
  tok_integer = -16,
  tok_real = -17,

  // extended tokens
  tok_lessOp = -18,
  tok_greaterOp = -19,
  tok_euqalOp = -20,
  tok_modOp = -21,
  tok_lSquareBracket = -22,
  tok_rSquareBracket = -23,
  tok_nEqualOp = -24,
  tok_function = -25,
  tok_extern = -26,
  tok_intType = -27,
  tok_stringType = -28,
  tok_doubleType = -29,
  tok_comma = -30,
  tok_return = -31,
  tok_voidType = -32,
  tok_charType = -33,
  tok_break = -34,

  tok_identifier = -35
};

// Token class
// token types are defined in js--.lex (and generated lex.yy.cc)
// since token value may be string, double, int and NULL(keywords)
// we simply use string, so we can translate them later
class Token {
public:
  int tp;

  std::string val;

  int lineNo;

  Token(int tokenType, std::string tokenValue, int lineNo) : tp(tokenType), val(std::move(tokenValue)), lineNo(lineNo) {}
};

#endif
