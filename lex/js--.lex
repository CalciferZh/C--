%option noyywrap
%{
  int mylineno = 0;
  enum {
    // keywords
    tok_var = -1, // maybe not used
    tok_if = -2,
    tok_else = -3,
    tok_while = -4,

    // symbols & operators
    tok_assignOp = -5,
    tok_semicolon = -6,
    tok_lParenthesis = -7,
    tok_rParenthesis = -8,
    tok_lBrace = -9,
    tok_rBrace = -10,
    tok_addOp = -11,
    tok_subtractOp = -12,
    tok_multiplyOp = -13,
    tok_divideOp = -14,

    // supported constant expression
    tok_string = -15,
    tok_integer = -16,
    tok_real = -17,


    // extended tokens
    tok_lessOp = -18,
    tok_greaterOp = -19,
    tok_euqalOp = -20,
    tok_modOp = -21,

    tok_identifier = -22    
  };
%}

string  \"[^\n"]+\"
ws	   [ \t\n]+
alpha   [A-Za-z]
dig     [0-9]
name	({alpha}|{dig}|\$)({alpha}|{dig}|[_.\-/$])*
int   [-+]?{dig}+
real  [-+]?{dig}*\.{dig}+

num1	[-+]?{dig}+\.?([eE][-+]?{dig}+)?
num2	[-+]?{dig}*\.{dig}+([eE][-+]?{dig}+)?
number  {num1}|{num2}

%%

{ws}                /* skip blanks and tabs */
var                 std::cout << tok_var << '\n';
if                  std::cout << tok_if << '\n';
else                std::cout << tok_else << '\n';
while               std::cout << tok_while << '\n';
=                   std::cout << tok_assignOp << '\n';
\;                   std::cout << tok_semicolon << '\n';
\(                   std::cout << tok_lParenthesis << '\n';
\)                   std::cout << tok_rParenthesis << '\n';  
\{                   std::cout << tok_lBrace << '\n';
\}                   std::cout << tok_rBrace << '\n';
\+                   std::cout << tok_addOp << '\n';
\-                   std::cout << tok_subtractOp << '\n';
\*                   std::cout << tok_multiplyOp << '\n';
\/                   std::cout << tok_divideOp << '\n';
{string}            std::cout << tok_string << '\n';
{int}               std::cout << tok_integer << '\n';
{real}              std::cout << tok_real << '\n';
\<                   std::cout << tok_lessOp << '\n';
\>                   std::cout << tok_greaterOp << '\n';
\==                  std::cout << tok_euqalOp << '\n';
\%                   std::cout << tok_modOp << '\n';
{name}              std::cout << tok_identifier << '\n';

%%

int main( int /* argc */, char** /* argv */ )
{
  FlexLexer* lexer = new yyFlexLexer;
  while (lexer->yylex() != 0);
  return 0;
}