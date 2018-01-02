#ifndef __LEXER_H__
#define __LEXER_H__

#include <vector>
#include <fstream>
#include <iostream>
#include <utility>
#include <FlexLexer.h>
#include "token.h"

// Lexer class
// specify source file path when constructed
// then call 'analyze'
class Lexer {
public:
  std::vector<Token> analyze();

  std::string src;

  std::ifstream srcStream;

  FlexLexer *worker;

  Lexer(std::string filePath) : src(filePath) {
    srcStream.open(filePath.c_str());
    worker = new yyFlexLexer(&srcStream);
  }

  ~Lexer() {delete worker;}
};


#endif