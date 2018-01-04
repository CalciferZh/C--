#include "parser.h"
#include "../lexer/lexer.h"

using namespace std;

int main()
{
  Lexer lexer("../naive.c");
  vector<Token> strm = lexer.analyze();
  Parser parser(strm);
  parser.parse();
  parser.print();
  return 0;
}

