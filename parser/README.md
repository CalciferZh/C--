# parser

## class Parser

### Overview

Parser receives a token stream when constructing, which is from lexer.
Then you can call `Parser::parse` to build the AST for the token stream.
After parsing, you can call `Parser::print` to see the constructed AST.

### Abstract Syntax Tree

Parser holds two important data members: `std::vector<std::unique_ptr<ExprAST>> expressions`
and `std::vector<std::unique_ptr<FunctionAST>> functions` to figure out
the program structure.

`expressions` holds all top-level senteces in order, and `functions` holds all user
defined functions. Note that we currently don't support in-function function definition.

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
