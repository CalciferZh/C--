#include "AST.h"
#include "../lexer/token.h"

llvm::Value* RealExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: RealExpr\n";
  return llvm::ConstanFP::get(context, llvm::APFloat(val));
}

llvm::Value* IntExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: IntExpr\n";
  return llvm::ConstantInt::get(context, llvm::APInt(32, val, true));
}

llvm::Value* StringExprAST(CODEGENPARM) {
  std::cout << "Generating: StringExpr\n";
  // Vica: Not done yet
}

llvm::Value* VariableExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: VariableExpr\n";
  // Vica: The type should be changed
  if (varTable.find(name) == varTable.end()) {
    std::cout << "Unknown variable\n";
    //varTable[name] = builder.CreateAlloca(llvm::Type::getInt32Ty(context), 0, nullptr, name.c_str());
  }
  return builder.CreateLoad(varTable[name], name.c_str());
}

llvm::Value* BinaryExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: BinaryExpr\n";
  llvm::Value* L = LHS->codegen(builder, varTable, context, module);
  llvm::Value* R = RHS->codegen(builder, varTable, context, module);
  if (!L || !R)
    return nullptr;
  bool isFloat = L->getType()->isFloatTy() || L->getType()->isFloatTy();
  // Vica: The type may be the problem
  switch (op) {
  case tok_addOp:
    return isFloat ? builder.CreateFAdd(L, R, "addtmp") : builder.CreateAdd(L, R, "addtmp");
  case tok_subtractOp:
    return isFloat ? builder.CreateFSub(L, R, "subtmp") : builder.CreateSub(L, R, "addtmp");
  case tok_multiplyOp:
    return isFloat ? builder.CreateFMul(L, R, "multmp") : builder.CreateMul(L, R, "addtmp");
  case tok_divideOp:
    return isFloat ? builder.CreateFDiv(L, R, "divtmp") : builder.CreateSDIV(L, R, "addtmp");
  case tok_lessOp:
    L = isFloat ? builder.CreateFCmpULT(L, R, "cmptmp") : builder.CreateICmpSLT(L, R, "cmptmp");
    return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context), "booltmp");
  case tok_greaterOp:
    L = isFloat ? builder.CreateFCmpUGT(L, R, "cmptmp") : builder.CreateICmpSGT(L, R, "cmptmp");
    return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context), "booltmp");
  case tok_euqalOp:
    L = isFloat ? builder.CreateFCmpUEQ(L, R, "eqtmp") : builder.CreateICmpEQ(L, R, "eqtmp");
    return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context), "booltmp");
  case tok_nEqualOp:
    L = isFloat ? builder.CreateFCmpUNE(L, R, "neqtmp") : builder.CreateICmpNE(L, R, "neqtmp");
    return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context), "booltmp");
  case tok_modOp:// Vica: not sure
    return isFloat ? builder.CreateFRem(L, R, "remtmp") : builder.CreateSRem(L, R, "remtmp");
  case tok_assignOp:
    return builder.CreateStore(R, L, false);
  default:
    std::cout << "invalid binary operator\n";
    return nullptr;
  }
}

llvm::Value* DeclareExprAST(CODEGENPARM) {
  std::cout << "Generating: DeclareExpr\n";
  llvm::Value* val = init->codegen(builder, varTable, context, module);
}

llvm::Function* PrototypeAST::codegen(CODEGENPARM) {
  std::cout << "Generating: Prototype\n";
  // Make the function type:  double(double,double) etc.
  std::vector<llvm::Type *> Doubles(args.size(), llvm::Type::getDoubleTy(context));
  llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(context), Doubles, false);
  llvm::Function* F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, module.get());

  // Set names for all arguments.
  unsigned Idx = 0;
  for (auto &arg : F->args())
    arg.setName(args[Idx++]);

  return F;
}

llvm::Function* FunctionAST::codegen(CODEGENPARM) {
  std::cout << "Generating: Function\n";
  // First, check for an existing function from a previous 'extern' declaration.
  llvm::Function* TheFunction = module->getFunction(proto->getName());

  if (!TheFunction)
    TheFunction = proto->codegen(builder, varTable, context, module);

  if (!TheFunction)
    return nullptr;

  // Create a new basic block to start insertion into.
  llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", TheFunction);
  builder.SetInsertPoint(BB);

  // Record the function arguments in the NamedValues map.
  // Vica: I dont know whether should I clear it
  // varTable.clear();
  for (auto &Arg : TheFunction->args())
    varTable[Arg.getName()] = &arg;

  if (llvm::Value* retVal = Body->codegen(builder, varTable, context, module)) {
    // Finish off the function.
    Builder.CreateRet(retVal);

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*TheFunction);

    return TheFunction;
  }

  // Error reading body, remove function.
  TheFunction->eraseFromParent();
  return nullptr;
}
