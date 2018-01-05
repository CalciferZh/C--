#include "parser.h"
#include "../lexer/lexer.h"

using namespace std;

int main() {
  Lexer lexer("../plalindrome/main.c");
  vector<Token> strm = lexer.analyze();
  Parser parser(strm);
  parser.parse();
  parser.print();
  parser.module->dump();
  return 0;
}

