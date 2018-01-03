var int1 = 23333 + 344444 + 455555 * 66666 + 77777 / 88888;
var int2 = 23333;
var int3 = int1 + int2 + 9608;
var int4 = (int1 < int2);
int3 = int4;

if (int3 < int4) {
  int3 = int4;
  int1 = int4;
} else {
  int4 = int3;
  int1 = int3;
}
