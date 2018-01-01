#include "AST.h"



llvm::Value* DeclareExprAST::codegen(llvm::IRBuilder<>& builder, std::map<std::string, llvm::AllocaInst *> varTable)
{
  llvm::Function* function = builder.GetInsertBlock()->getParent();
  llvm::Value* initVal = init->codegen(builder, varTable);
  llvm::AllocaInst* alloca = CreateEntryBlockAlloca(function, varName);
  builder.CreateStore(initVal, alloca);
  varTable[varName] = alloca;
}
