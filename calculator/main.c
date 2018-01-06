#include <stdio.h>
#include <stdlib.h>

int isOp(char ch) {
  if (ch == '+') {
    return 1;
  }
  if (ch == '-') {
    return 2;
  }
  if (ch == '*') {
    return 3;
  }
  if (ch == '/') {
    return 4;
  }
  if (ch == '(') {
    return 5;
  }
  if (ch == ')') {
    return 6;
  }
  return 0;
}

int main() {
  char rawStr[128] = "1+(5-2)*4/(2+1)";
  int rawStrLen = 15;
  int rawStrIdx = 0;

  // polish expression
  int exprStack[128];
  int isOpEle[128];
  int exprStackHead = 0;
  int exprStackIdx = 0;

  // final result
  int numStack[128];
  int numStackHead = 0;

  int opStack[128];
  int opStackHead = 0;

  int tmpSymbol;
  int lhs = 0;
  int rhs = 0;
  int res = 0;

  int i;

  opStack[opStackHead] = 5;
  opStackHead = opStackHead + 1;

  while (rawStrIdx < rawStrLen) {
    tmpSymbol = isOp(rawStr[rawStrIdx]);
    if (tmpSymbol == 0) {
      exprStack[exprStackHead] = rawStr[rawStrIdx] - '0';
      isOpEle[exprStackHead] = 0;
      exprStackHead = exprStackHead + 1;
    }
    if (tmpSymbol == 1 || tmpSymbol == 2 || tmpSymbol == 3 || tmpSymbol == 4) {
      if (opStack[opStackHead - 1] == 6 || opStack[opStackHead - 1] == 5 || tmpSymbol == 3 || tmpSymbol == 4)
      {
        opStack[opStackHead] = tmpSymbol;
        opStackHead = opStackHead + 1;
      } else {
        opStackHead = opStackHead - 1;
        exprStack[exprStackHead] = opStack[opStackHead];
        isOpEle[exprStackHead] = 1;
        exprStackHead = exprStackHead + 1;
        opStack[opStackHead] = tmpSymbol;
        opStackHead = opStackHead + 1;
      }
    }
    if (tmpSymbol == 5) {
      opStack[opStackHead] = tmpSymbol;
      opStackHead = opStackHead + 1;
    }
    if (tmpSymbol == 6) {
      opStackHead = opStackHead - 1;
      while (opStack[opStackHead] != 5) {
        exprStack[exprStackHead] = opStack[opStackHead];
        isOpEle[exprStackHead] = 1;
        exprStackHead = exprStackHead + 1;
        opStackHead = opStackHead - 1;
      }
    }
    rawStrIdx = rawStrIdx + 1;
  }

  // we use 1 since opStack[0] is '('
  while (opStackHead != 1) {
    opStackHead = opStackHead - 1;
    exprStack[exprStackHead] = opStack[opStackHead];
    isOpEle[exprStackHead] = 1;
    exprStackHead = exprStackHead + 1;
  }
  while (exprStackIdx != exprStackHead) {
    if (isOpEle[exprStackIdx]) {
      rhs = numStack[numStackHead - 1];
      lhs = numStack[numStackHead - 2];
      numStackHead = numStackHead - 2;
      if (exprStack[exprStackIdx] == 1) {
        res = lhs + rhs;
      }
      if (exprStack[exprStackIdx] == 2) {
        res = lhs - rhs;
      }
      if (exprStack[exprStackIdx] == 3) {
        res = lhs * rhs;
      }
      if (exprStack[exprStackIdx] == 4) {
        res = lhs / rhs;
      }
      numStack[numStackHead] = res;
      numStackHead = numStackHead + 1;
    } else {
      numStack[numStackHead] = exprStack[exprStackIdx];
      numStackHead += 1;
    }
    exprStackIdx += 1;
  }
  printf("%d\n", numStack[0]);
  return 0;
}

