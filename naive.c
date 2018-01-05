int max(int a, int b) {
  if (a < b) {
    return a;
  } else {
    return b;
  }
}

int main(int argc, string argv) {
  int a = 1;
  int b = a + 2;
  int c = b + a * 3;
  string d = "A long string";
  double e = 3.1415;
  int f = max(b, a);
  int g[100];
  int h;
  int i[2] = 1;

  if (a < b)
  {
    a = b;
  } else {
    a = c;
    b = c;
  }

  while (a > 0) {
    a = a - 1;
  }

  return 0;
}
