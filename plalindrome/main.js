var string1 = "this is some fxxking string";
var string2 = "this is a si siht";
var len1 = 27;
var len2 = 17;
var i = 0;
var flag = 1;
while (i < len2) {
  if (string2[i] != string2[len2 - i]) {
    flag = 0;
  }
}

if (flag == 0) {
  print("Yes");
} else {
  print("No");
}
