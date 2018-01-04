int a = 1;
int b = a + 2;
int c = b + a * 3;
string d = "A long string";

if (a < b) {
  a = b;
} else {
  a = c;
  b = c;
}

while (a > 0) {
  a = a - 1;
}
