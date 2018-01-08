#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <string>
#include <iostream>

class Variable {
public:
  std::string name;

  int tp;

  //llvm::AllocaInst* addr;
  llvm::Value* addr;
  //Variable(std::string name, int tp, llvm::AllocaInst* addr = nullptr) : name(std::move(name)), tp(tp), addr(addr) {}
  Variable(std::string name, int tp, llvm::Value* addr = nullptr) : name(std::move(name)), tp(tp), addr(addr) {}
  void print() {
    std::cout << "(" << tp << ")" << name << '\n';
  }
};

#endif
