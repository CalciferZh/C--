#include "parser.h"
#include "../lexer/lexer.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc == 1) {
    std::cout << "Need filename";
    return 0;
  }
  if (argc > 2) {
    std::cout << "Too many arguments";
    return 0;
  }
  Lexer lexer(argv[1]);
  vector<Token> strm = lexer.analyze();
  Parser parser(strm);
  parser.parse();
  for (auto& ptr : parser.functions) {
    try {
      ptr->codegen(parser.builder, parser.varTable, parser.context ,parser.module);
    } catch (CodegenException &e) {
      e.print();
    }
  }
  parser.module->print(llvm::errs(), nullptr);
  return 0;
}
