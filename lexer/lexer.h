#ifndef __LEXER_H__
#define __LEXER_H__

#include <vector>
#include <fstream>
#include <iostream>
#include <utility>
#include <FlexLexer.h>

// Token class
// token types are defined in js--.lex (and generated lex.yy.cc)
// since token value may be string, double, int and NULL(keywords)
// we simply use string, so we can translate them later
class Token {
public:
  Token(int tokenType, std::string tokenValue) : tp(tokenType), val(std::move(tokenValue)) {}
  int tp;
  std::string val;
};

// Lexer class
// specify source file path when constructed
// then call 'analyze'
class Lexer {
public:
  Lexer(std::string filePath) : src(filePath) {
    srcStream.open(filePath.c_str());
    worker = new yyFlexLexer(&srcStream);
  }
  ~Lexer() {delete worker;}
  std::vector<Token> analyze();
  std::string src;
  std::ifstream srcStream;
  FlexLexer* worker;
};


#endif