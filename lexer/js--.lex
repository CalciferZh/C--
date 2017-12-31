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
    tok_lSquareBracket = -22,
    tok_rSquareBracket = -23,
    tok_nEqualOp = -24,

    tok_identifier = -25
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

{ws}                  /* skip blanks and tabs */
var                   return(tok_var);
if                    return(tok_if);
else                  return(tok_else);
while                 return(tok_while);
=                     return(tok_assignOp);
\;                    return(tok_semicolon);
\(                    return(tok_lParenthesis);
\)                    return(tok_rParenthesis);  
\{                    return(tok_lBrace);
\}                    return(tok_rBrace);
\+                    return(tok_addOp);
\-                    return(tok_subtractOp);
\*                    return(tok_multiplyOp);
\/                    return(tok_divideOp);
{string}              return(tok_string);
{int}                 return(tok_integer);
{real}                return(tok_real);
\<                    return(tok_lessOp);
\>                    return(tok_greaterOp);
\==                   return(tok_euqalOp);
\!=                   return(tok_nEqualOp);
\%                    return(tok_modOp);
{name}                return(tok_identifier);
\[                    return(tok_lSquareBracket);
\]                    return(tok_rSquareBracket);

%%
