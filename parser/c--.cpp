#include "parser.h"
#include "../lexer/lexer.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc == 1) {
    std::cout << "Need filename";
    return 0;
  }
  Lexer lexer(argv[1]);
  vector<Token> strm = lexer.analyze();
  Parser parser(strm);
  parser.parse();
  parser.generate();
  return 0;
}
