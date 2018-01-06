#include "lexer.h"
#include <cstring>
using namespace std;

vector<Token> Lexer::analyze() {
  int tkType;
  vector<Token> tkStream;
  while ((tkType = worker->yylex()) != 0) {
    auto text = worker->YYText();
    auto len = std::strlen(text);
    if (tkType == tok_string || tkType == tok_char) {
      ++text; // eat heading " or '
      len -= 2; // eat  trilling " or '
    }
    tkStream.emplace_back(tkType, string(text, len), worker->lineno());
  }
  return tkStream;
}

