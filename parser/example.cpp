#include "parser.h"
#include "../lexer/lexer.h"

using namespace std;

int main(int argc, char* argv[]) {
  //Lexer lexer("../calculator/main.c");
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
  //parser.print();
  for (auto& ptr : parser.functions) {
    ptr->codegen(parser.builder, parser.varTable, parser.context ,parser.module);
  }
  parser.module->print(llvm::errs(), nullptr);
  return 0;
}

