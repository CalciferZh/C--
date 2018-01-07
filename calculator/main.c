extern void printd(double val);
// #include <stdio.h>

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
  char rawStr[128] = "1+(52-2)*4/(2+1)";
  int rawStrLen = 16;
  int rawStrIdx = 0;

  // polish expression
  int exprStack[128];
  int isOpEle[128];
  int exprStackHead = 0;
  int exprStackIdx = 0;

  // final result
  double numStack[128];
  int numStackHead = 0;

  int opStack[128];
  int opStackHead = 0;

  int tmpSymbol;
  double lhs = 0;
  double rhs = 0;
  double res = 0;

  int tmp;
  int isLastEleNum = 0;

  opStack[opStackHead] = 5;
  opStackHead = opStackHead + 1;

  while (rawStrIdx < rawStrLen) {
    tmpSymbol = isOp(rawStr[rawStrIdx]);
    if (tmpSymbol == 0) {
      tmp = rawStr[rawStrIdx] - '0';
      if (isLastEleNum) {
        // if last element is also a number
        // the two number should be 'combined'
        exprStack[exprStackHead - 1] = exprStack[exprStackHead - 1] * 10 + tmp;
      } else {
        exprStack[exprStackHead] = tmp;
        isOpEle[exprStackHead] = 0;
        exprStackHead = exprStackHead + 1;
      }
    }
    if (tmpSymbol == 1 || tmpSymbol == 2 || tmpSymbol == 3 || tmpSymbol == 4) {
      if (opStack[opStackHead - 1] == 5 || opStack[opStackHead - 1] == 6 || tmpSymbol == 3 || tmpSymbol == 4) {
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
    if (tmpSymbol == 0) {
      isLastEleNum = 1;
    } else {
      isLastEleNum = 0;
    }

    // printf("=================================================\n");
    // for (tmp = 0; tmp < exprStackHead; ++tmp) {
    //   printf("%d %d\n", exprStack[tmp], isOpEle[tmp]);
    // }
    // printf("-------------------------------------------------\n");
    // for (tmp = 0; tmp < opStackHead; ++tmp) {
    //   printf("%d\n", opStack[tmp]);
    // }
  }

  // we use 1 since opStack[0] is '('
  while (opStackHead != 1) {
    opStackHead = opStackHead - 1;
    exprStack[exprStackHead] = opStack[opStackHead];
    isOpEle[exprStackHead] = 1;
    exprStackHead = exprStackHead + 1;
  }

  // printf("=================================================\n");
  // for (tmp = 0; tmp < exprStackHead; ++tmp) {
  //   printf("%d %d\n", exprStack[tmp], isOpEle[tmp]);
  // }

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
      numStackHead = numStackHead + 1;
    }
    exprStackIdx = exprStackIdx + 1;
  }
  printd(numStack[0]);
  // printf("%f", numStack[0]);
  return 0;
}

