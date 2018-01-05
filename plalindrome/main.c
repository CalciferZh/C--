int main() {
  char str[32] = "a sys a";
  int i = 0;
  int len = 32;
  int flag = 0;
  while (i < len) {
    if (str[i] != str[len - i - 1]) {
      flag = 1;
    }
    i = i + 1;
  }
  return 0;
}