#include "lexer.h"

using namespace std;

int main() {
  Lexer lexer("../calculator/main.c");
  vector<Token> strm = lexer.analyze();
  for (auto t : strm) {
    std::cout << "TYPE: " << t.tp
              << "\tVALUE: " << t.val
              << "\tLINENO: " << t.lineNo << '\n';
  }
  return 0;
}

