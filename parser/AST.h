#ifndef __AST_H__
#define __AST_H__

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "variable.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#define CODEGENPARM llvm::IRBuilder<>& builder, std::map<std::string, std::unique_ptr<Variable>>& varTable, llvm::LLVMContext& context, std::unique_ptr<llvm::Module>& module

class ExprAST {
public:
  virtual ~ExprAST() = default;

  virtual void print() = 0;

  virtual llvm::Value* codegen(CODEGENPARM) = 0;

  virtual int getClassType() = 0;
};

class RealExprAST : public ExprAST {
public:
  double val;

  RealExprAST(double val) : val(val) {}

  void print() override {
    std::cout << "Real number: " << val << '\n';
  }

  llvm::Value* codegen(CODEGENPARM) override;

  int getClassType() override { return 1; }
};

class IntExprAST : public ExprAST {
public:
  int val;

  IntExprAST(int val) : val(val) {}

  void print() override {
    std::cout << "Integer: " << val << '\n';
  }

  llvm::Value *codegen(CODEGENPARM) override;

  int getClassType() override { return 2; }
};

class StringExprAST : public ExprAST {
public:
  std::string val;

  StringExprAST(const std::string& val) : val(val) {}

  void print() override {
    std::cout << "String: " << val << '\n';
  }

  llvm::Value *codegen(CODEGENPARM) override;

  int getClassType() override { return 3; }
};

class CharExprAST : public ExprAST {
public:
  char val;

  CharExprAST(const std::string &val) : val(val[0]) {}

  void print() override {
    std::cout << "Char: " << val << '\n';
  }

  llvm::Value *codegen(CODEGENPARM) override;

  int getClassType() override { return 4; }
};

class VariableExprAST : public ExprAST {
public:
  std::string name;

  std::unique_ptr<ExprAST> offset;

  VariableExprAST(const std::string& name) : name(name), offset(nullptr) {}

  VariableExprAST(const std::string& name, std::unique_ptr<ExprAST> offset) : name(name), offset(std::move(offset)) {}

  void print() override {
    std::cout << "Variable: {\n";

    std::cout << "name: " << name << '\n';

    std::cout << "offset: {\n";
    if (offset) {
      offset->print();
    } else {
      std::cout << "null\n";
    }
    std::cout << "}\n}\n";
  }

  llvm::Value* codegen(CODEGENPARM) override;

  int getClassType() override { return 5; }
};

class BinaryExprAST : public ExprAST {
public:
  // in lexer/js--.lex : enum
  int op;

  std::unique_ptr<ExprAST> LHS;

  std::unique_ptr<ExprAST> RHS;

  BinaryExprAST(int op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS) : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  void print() override {
    std::cout << "expression: " << op << " { \nLHS: {\n";
    if (LHS) {
      LHS->print();
    } else {
      std::cout << "nullptr\n";
    }
    std::cout << "}\nRHS: {\n";
    if (RHS) {
      RHS->print();
    } else {
      std::cout << "nullptr\n";
    }
    std::cout << "}\n}\n";
  }

  llvm::Value* codegen(CODEGENPARM) override;

  int getClassType() override { return 6; }
};

class DeclareExprAST : public ExprAST {
public:
  int tp;

  std::string name;

  std::unique_ptr<ExprAST> init = nullptr;

  int size = 0;

  DeclareExprAST(int tp, std::string name, int size, std::unique_ptr<ExprAST> init) : tp(tp), name(std::move(name)), init(std::move(init)), size(size) {}

  void print() override {
    std::cout << "Declaration for " << tp << ": { \n" << "name: " << name;
    std::cout << "\nsize: ";
    std::cout << size;
    std::cout << "\ninit: {\n";
    if (init) {
      init->print();
    } else {
      std::cout << "None\n";
    }
    std::cout << "}\n}\n";
  }

  llvm::Value* codegen(CODEGENPARM) override;

  int getClassType() override { return 7; }
};

class IfExprAST : public ExprAST {
public:
  std::unique_ptr<ExprAST> cond;

  std::vector<std::unique_ptr<ExprAST>> ifBody;

  std::vector<std::unique_ptr<ExprAST>> elseBody;

  IfExprAST(std::unique_ptr<ExprAST> cond, std::vector<std::unique_ptr<ExprAST>> ifBody, std::vector<std::unique_ptr<ExprAST>> elseBody) : cond(std::move(cond)), ifBody(std::move(ifBody)), elseBody(std::move(elseBody)) {}

  void print() override {
    std::cout << "If-else: {\n" << "condition: {\n";
    cond->print();
    std::cout << "}\nif body: {\n";
    for (const auto& expr: ifBody) {
      expr->print();
    }
    std::cout << "}\nelse body: {\n";
    for (const auto &expr : elseBody) {
      expr->print();
    }
    std::cout << "}\n}\n";
  }

  llvm::Value* codegen(CODEGENPARM) override;

  int getClassType() override { return 8; }
};

class WhileExprAST : public ExprAST {
public:
  std::unique_ptr<ExprAST> cond;

  std::vector<std::unique_ptr<ExprAST>> body;

  WhileExprAST(std::unique_ptr<ExprAST> cond, std::vector<std::unique_ptr<ExprAST>> body) : cond(std::move(cond)), body(std::move(body)) {}

  void print() override {
    std::cout << "While: {\n" << "condition:\n";
    cond->print();
    std::cout << "body:{\n";
    for (const auto& expr: body) {
      expr->print();
    }
    std::cout << "}\n}\n";
  }

  llvm::Value* codegen(CODEGENPARM) override;

  int getClassType() override { return 9; }
};

class ReturnExprAST : public ExprAST {
public:
  std::unique_ptr<ExprAST> retExpr;

  ReturnExprAST(std::unique_ptr<ExprAST> retExpr) : retExpr(std::move(retExpr)) {}

  void print() override {
    std::cout << "return: {\n";
    retExpr->print();
    std::cout << "}\n";
  }

  llvm::Value* codegen(CODEGENPARM) override;

  int getClassType() override { return 10; }
};

class CallExprAST : public ExprAST {
public:
  std::string callee;

  std::vector<std::unique_ptr<ExprAST>> params;

  CallExprAST(std::string callee, std::vector<std::unique_ptr<ExprAST>> params) : callee(callee), params(std::move(params)) {}

  void print() override {
    std::cout << "Call: {\n" << "callee: " << callee << "\nparams: {\n";
    for (const auto& expr: params) {
      expr->print();
    }
    std::cout << "}\n}\n";
  }

  llvm::Value* codegen(CODEGENPARM) override;

  int getClassType() override { return 11; }
};

class BreakExprAST : public ExprAST {
public:
  BreakExprAST() = default;

  void print() override {
    std::cout << "Break expression.\n";
  }
  llvm::Value* codegen(CODEGENPARM) override;

  int getClassType() override { return 12; }
};

class InitListExprAST : public ExprAST {
public:
  std::vector<std::unique_ptr<ExprAST>> initList;

  InitListExprAST(std::vector<std::unique_ptr<ExprAST>> initList) : initList(std::move(initList)) {}

  void print() override {
    std::cout << "Init-list:\n";
    for (const auto& expr: initList) {
      expr->print();
    }
  }

  llvm::Value* codegen(CODEGENPARM) override { return nullptr; }

  int getClassType() override { return 13; }
};

class PrototypeAST {
public:
  std::string name;

  std::vector<std::unique_ptr<Variable>> args;

  int retType;

  PrototypeAST(const std::string &name, std::vector<std::unique_ptr<Variable>> args, int retType) : name(name), args(std::move(args)), retType(retType) {}

  void print() {
    std::cout << "Function prototype: { \n" << "name: " << name << " \nargs:\n";
    for (const auto& arg: args) {
      arg->print();
    }
    std::cout << "return type: " << retType << '\n';
    std::cout << "} \n";
  }

  llvm::Function* codegen(CODEGENPARM);
};

class FunctionAST {
public:
  std::unique_ptr<PrototypeAST> proto;

  std::vector<std::unique_ptr<ExprAST>> body;

  FunctionAST(std::unique_ptr<PrototypeAST> proto, std::vector<std::unique_ptr<ExprAST>> body) : proto(std::move(proto)), body(std::move(body)) {}

  void print() {
    std::cout << "Function: {\n" << "prototype: {\n";
    proto->print();
    std::cout << "}\nbody: {\n";
    for (const auto& expr: body) {
      expr->print();
    }
    std::cout << "}\n}\n";
  }
  llvm::Function* codegen(CODEGENPARM);
};

#endif
