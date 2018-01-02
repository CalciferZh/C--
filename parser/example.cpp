#include "parser.h"
#include "../lexer/lexer.h"

using namespace std;

int main()
{
  Lexer lexer("../naive.js");
  vector<Token> strm = lexer.analyze();
  Parser parser(strm);
  parser.parse();
  return 0;
}

