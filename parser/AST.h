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
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>


class ExprAST {
public:
  virtual ~ExprAST() = default;

  virtual void print() = 0;

  // virtual llvm::Value* codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) = 0;
};

class RealExprAST : public ExprAST {
public:
  double val;

  RealExprAST(double val) : val(val) {}

  void print() override {
    std::cout << "Real number: " << val << '\n';
  }

  // llvm::Value* codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class IntExprAST : public ExprAST
{
public:
  int val;

  IntExprAST(int val) : val(val) {}

  void print() override {
    std::cout << "Integer: " << val << '\n';
  }

  // llvm::Value *codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class StringExprAST : public ExprAST
{
public:
  std::string val;

  StringExprAST(const std::string& val) : val(val) {}

  void print() override {
    std::cout << "String: " << val << '\n';
  }

  // llvm::Value *codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class VariableExprAST : public ExprAST {
public:
  std::string name;

  VariableExprAST(const std::string& name) : name(name) {}

  void print() override {
    std::cout << "variable: " << name << '\n';
  }

  // llvm::Value* codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
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
    LHS->print();
    std::cout << "}\nRHS: {\n";
    RHS->print();
    std::cout << "}\n}\n";
  }

  // llvm::Value* codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class DeclareExprAST : public ExprAST {
public:
  std::unique_ptr<ExprAST> var;
  
  std::unique_ptr<ExprAST> init;

  DeclareExprAST(std::unique_ptr<ExprAST> var, std::unique_ptr<ExprAST> init) : var(std::move(var)), init(std::move(init)) {}

  void print() override {
    std::cout << "Declaration: { \n" << "var: "; 
    var->print();
    std::cout << "\ninit: {\n";
    init->print();
    std::cout << "}\n}\n";
  }

  // llvm::Value* codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class WhileExprAST : public ExprAST {
public:
  std::unique_ptr<ExprAST> cond;

  std::vector<std::unique_ptr<ExprAST>> body;

  WhileExprAST(std::unique_ptr<ExprAST> cond, std::vector<std::unique_ptr<ExprAST>> body) : cond(std::move(cond)), body(std::move(body)) {}

  void print() {
    std::cout << "While prototype: {\n" << "condition:\n";
    cond->print();
    std::cout << "body:{\n";
    for (const auto& expr: body) {
      expr->print();
    }
    std::cout << "}\n}\n";
  }

  // llvm::Value* codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class PrototypeAST {
public:
  std::string name;

  std::vector<std::string> args;

  PrototypeAST(const std::string& name, std::vector<std::string> args) : name(name), args(std::move(args)) {}

  void print() {
    std::cout << "Function prototype: { \n" << "name: " << name << " \nargs:\n";
    for (const auto& arg: args) {
      std::cout << arg << ", ";
    }
    std::cout << "} \n";
  }

  // llvm::Function* codegen();
};

class FunctionAST {
public:
  std::unique_ptr<PrototypeAST> proto;

  std::vector<std::unique_ptr<ExprAST>> body;

  FunctionAST(std::unique_ptr<PrototypeAST> proto, std::vector<std::unique_ptr<ExprAST>> body) : proto(std::move(proto)), body(std::move(body)) {}

  void print() {
    std::cout << "Function: {\n" << "prototype:\n";
    proto->print();
    std::cout << "body: {\n";
    for (const auto& expr: body) {
      expr->print();
    }
    std::cout << "}\n}\n";
  }
  // llvm::Function* codegen();
};

#endif
