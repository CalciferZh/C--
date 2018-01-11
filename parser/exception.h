#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <exception>
#include <string>

class ParseException {
public:
  static constexpr int MSG_BUF_LEN = 512;

  char msg[MSG_BUF_LEN];

  ParseException(const char* expected, const char* got, int lineNo) {
    snprintf(msg, MSG_BUF_LEN, "Error at line %d: expected %s, got %s.\n", lineNo, expected, got);
  }

  void print() {
    std::cerr << msg;
  }
};

class CodegenException {
public:

  std::string msg;

  CodegenException(std::string msg) : msg(msg){}

  void print() {
    std::cerr << msg << std::endl;
  }
};

#endif
