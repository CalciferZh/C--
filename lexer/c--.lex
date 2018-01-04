%option noyywrap
%{
#include "token.h"
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
int                   return{tok_intType};
double                return{tok_doubleType};
string                return{tok_stringType};
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
