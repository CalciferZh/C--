int main() {
  char str[32] = "a sys a";
  int i = 0;
  int len = 32;
  int flag = 0;
  while (i < len) {
    // flag = 1 if not a plalindrome
    if (str[i] != str[len - i - 1]) {
      flag = 1;
      break;
    }
    i = i + 1;
  }
  if (flag == 1) {
    printf("false");
  } else {
    printf("true");
  }
  return 0;
}