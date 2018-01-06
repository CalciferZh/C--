#include "parser.h"
#include "../lexer/lexer.h"

using namespace std;

int main() {
  Lexer lexer("../calculator/main.c");
  vector<Token> strm = lexer.analyze();
  Parser parser(strm);
  parser.parse();
  parser.print();
  for (auto& ptr : parser.functions) {
    ptr->codegen(parser.builder, parser.varTable, parser.context ,parser.module);
  }
  //parser.module->print(llvm::errs(), nullptr);
  return 0;
}

