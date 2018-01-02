#ifndef __PARSER_H__
#define __PARSER_H__

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


class ExprAST {
public:
  virtual ~ExprAST() = default;

  virtual llvm::Value* codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) = 0;
};

class RealExprAST : public ExprAST {
public:
  double val;

  RealExprAST(double val) : val(val) {}

  llvm::Value* codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class IntExprAST : public ExprAST
{
public:
  int val;

  IntExprAST(int val) : val(val) {}

  llvm::Value *codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class StringExprAST : public ExprAST
{
public:
  std::string val;

  StringExprAST(const std::string& val) : val(val) {}

  llvm::Value *codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class VariableExprAST : public ExprAST {
public:
  std::string name;

  VariableExprAST(const std::string& name) : name(name) {}

  llvm::Value* codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class BinaryExprAST : public ExprAST {
public:
  // in lexer/js--.lex : enum
  int op;
  std::unique_ptr<ExprAST> LHS;
  std::unique_ptr<ExprAST> RHS;

  BinaryExprAST(int op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS) : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  llvm::Value* codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class DeclareExprAST : public ExprAST {
public:
  std::string varName;
  
  std::unique_ptr<ExprAST> init;

  DeclareExprAST(std::string varName, std::unique_ptr<ExprAST> init) : varName(std::move(varName)), init(std::move(init)) {}

  llvm::Value* codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst*> varTable) override;
};

class PrototypeAST {
public:
  std::string name;

  std::vector<std::string> args;

  PrototypeAST::PrototypeAST(const std::string& name, std::vector<std::string> args) : name(name), args(std::move(args)) {}

  llvm::Function* codegen();
};

class FunctionAST {
public:
  std::unique_ptr<PrototypeAST> proto;

  std::vector<std::unique_ptr<ExprAST>> body;

  FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> proto, std::vector<std::unique_ptr<ExprAST>> body) : proto(std::move(proto)), body(std::move(body)) {}

  llvm::Function* codegen();
};

#endif