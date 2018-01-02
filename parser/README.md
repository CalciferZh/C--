# parser

## class Parser

### Overview

Parser receives a token stream when constructing, which is from lexer.
Then you can call `Parser::parse` to build the AST for the token stream.
After parsing, you can call `Parser::print` to see the constructed AST.

### Deduction Rules

In `Parser::parse()`, parser looply parse top-level sentences. The deduction rules are:

* top-level -> statement | function | extern

* statement -> declare | assign | while | if-else

* declare -> 'var' identifier '=' expression

* assign -> identifier '=' expression

* expression -> primary op primary

* primary -> '(' expression ')' |  string | integer | real

* function -> prototype body

* prototype -> identifier '(' identifier* ')'

* body -> expression*

### Data Member

Parser holds two important data members: `std::vector<std::unique_ptr<ExprAST>> expressions`
and `std::vector<std::unique_ptr<FunctionAST>> functions` to figure out
the program structure.

`expressions` holds all top-level senteces in order, and `functions` holds all user
defined functions. Note that in-function function definition will also viewed as global.

### Variables

In AST node we save variable's name, and we can look for more details via
`std::map<std::string, std::unique_ptr<Variable>> varTable`.

### Parse

In `Parse::parse()`, the parse will scan the token stream, and
push the result back to `expressions` or `functions`, depending
on the type of the sentence.

## class AST

Basically, we have 3 types of AST node: `ExprAST`, `PrototypeAST` and `FunctionAST`.

Common sencetences are viewed as expressions, except for function definitions.

`ExprAST` is derived by many types of AST nodes.

`FucntionAST` is composed of `ProtoypeAST` and a vector of `ExprAST`s.

### Print Format

Each AST have method `print`, which will print its structure.
For example, if you parse `var int1 = 23333 + 344444 + 455555 * 66666 + 77777 / 88888;`,
you will get following(after manually indented):

```js
Declaration: {
  varName: int1
  init: {
    expression: -11 {
      LHS: {
        expression: -11 {
          LHS: {
            expression: -11 {
              LHS: {
                Integer: 23333
              }
              RHS: {
                Integer: 344444
              }
            }
          }
          RHS: {
            expression: -13 {
              LHS: {
                Integer: 455555
              }
              RHS: {
              initnteger: 66666
              }
            }
          }
        }
      }
      RHS: {
        expression: -14 {
          LHS: {
            Integer: 77777
          }
          RHS: {
            Integer: 88888
          }
        }
      }
    }
  }
}
```
