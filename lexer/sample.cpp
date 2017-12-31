#include "lexer.h"

using namespace std;

int main()
{
  Lexer lexer("../plalindrome/main.js");
  vector<Token> strm = lexer.analyze();
  std::cout << '\n';
  for (auto t : strm) {
    std::cout << "TYPE: " << t.tp
              << " VALUE: " << t.val << '\n';
  }
  return 0;
}

