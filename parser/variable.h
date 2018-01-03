#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <string>

class Variable {
public:
  std::string name;
  int tp;
  llvm::AllocaInst* addr;
};

#endif
