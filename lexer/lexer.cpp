#include "lexer.h"

using namespace std;

vector<Token> Lexer::analyze()
{
  int tkType;
  vector<Token> tkStream;
  while ((tkType = worker->yylex()) != 0) {
    tkStream.emplace_back(tkType, string(worker->YYText()));
  }
  return tkStream;
}

