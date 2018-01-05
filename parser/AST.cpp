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
  if (varTable.find(name) != varTable.end()) {
    std::cout << "Unknown variable\n";
    return nullptr;
  }
  // Vica: Need to handle offset
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
  if (varTable.find(name) != varTable.end()) {
    std::cout << "Variable redefinition\n";
    return nullptr;
  } else {
    // Vica: The type should be changed in time
    llvm::Type* type;
    switch (tp) {
      case tok_intType:
        type = llvm::Type::getInt32Ty(context);        
        break;
      case tok_doubleType:
        type = llvm::Type::getDoubleTy(context)
        break;
      case tok_charType:
      case toke_stringType:
        type = llvm::Type::getInt8Ty(context);
        break;
      default:
        type = llvm::Type::getInt32PtrTy(context);
        break;
    }
    if (size != 0) {
      type = llvm::ArrayType::get(type, size);
    }
    varTable[name] = builder.CreateAlloca(type, 0, nullptr, name.c_str());
  }
  if (var != nullptr) {
    llvm::Value* L = builder.CreateLoad(varTable[name], name.c_str());
    llvm::Value* R = var->codegen(builder, varTable, context, module);
    if (!L) {
      std::cout << "Error when loading variable\n";
      return nullptr;
    }
    if (!R) {
      std::cout << "Invalid expression\n";
      return nullptr;
    }
    if (offset != 0) {
      // Vica: init array? 
    } else {
      builder.CreateStore(R, L);
    }
    return R;
  }
  /*
  VariableExprAST *LHSE = dynamic_cast<VariableExprAST*>(var.get());
  if (!LHSE) {
    std::cout << "LHS must be a variable\n";
    return nullptr;
  }
  llvm::Value* R = init->codegen(builder, varTable, context, module);
  if (!R) {
    std::cout << "Invalid Expression\n";
    return nullptr;
  }
  llvm::Value* L = varTable[LHSE->getName()];
  if (!L) {
    std::cout << "Unknown Variable\n";
    return nullptr;
  }
  builder.CreateStore(R, L);
  return R;
  */
}

llvm::Value* IfExprAST::codegen(CODEGENPARM) {
  // Vica: they are vector
  llvm::Value* CondV = cond->codegen(builder, varTable, context, module);
  if (!CondV) {
    std::cout << "Error in condition\n";
    return nullptr;
  }
  CondV = builder.CreateFCmpONE(CondV, ConstantFP::get(context, APFloat(0.0)), "ifcond");
  // Vica: why use function here?
  llvm::Function *func = builder.GetInsertBlock()->getParent();
  // Create blocks for the then and else cases.  Insert the 'then' block at the end of the function.
  llvm::BasicBlock* ThenBB = llvm::BasicBlock::Create(context, "then", func);
  llvm::BasicBlock* ElseBB = llvm::BasicBlock::Create(context, "else");
  llvm::BasicBlock* MergeBB = llvm::BasicBlock::Create(context, "ifcont");
  builder.CreateCondBr(CondV, ThenBB, ElseBB);
  // Emit then value.
  builder.SetInsertPoint(ThenBB);
  Value* ThenV = then->codegen(builder, varTable, context, module);
  if (!ThenV) {
    std::cout << "Error in then\n";
    return nullptr;
  }
  builder.CreateBr(MergeBB);
  // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
  ThenBB = builder.GetInsertBlock();
  // Emit else block.
  func->getBasicBlockList().push_back(ElseBB);
  builder.SetInsertPoint(ElseBB);
  Value *ElseV = else->codegen(builder, varTable, context, module);
  if (!ElseV) {
    std::cout << "Error in else\n";
    return nullptr;
  }
  builder.CreateBr(MergeBB);
  // codegen of 'Else' can change the current block, update ElseBB for the PHI.
  ElseBB = builder.GetInsertBlock();
  // Emit merge block.
  func->getBasicBlockList().push_back(MergeBB);
  builder.SetInsertPoint(MergeBB);
  llvm::PHINode *PN = builder.CreatePHI(Type::getDoubleTy(TheContext), 2, "iftmp");

  PN->addIncoming(ThenV, ThenBB);
  PN->addIncoming(ElseV, ElseBB);
  return PN;
}

llvm::Value* WhileExprAST::codegen(CODEGENPARM) {
  llvm::BasicBlock* CondBB = llvm::BasicBlock::Create(context, "cond");
  llvm::BasicBlock* BodyBB = llvm::BasicBlock::Create(context, "body");
  llvm::BasicBlock* FiniBB = llvm::BasicBlock::Create(context, "finish");
  builder.CreateBr(CondBB);
  // Emit cond value
  builder.SetInsertPoint(CondBB);
  llvm::Value* CondV = cond->codegen(builder, varTable, context, module);
  if (!CondV) {
    std::cout << "Error in condition\n";
    return nullptr;
  }
  CondV = builder.CreateFCmpONE(CondV, ConstantFP::get(context, APFloat(0.0)), "whilecond");
  builder.CreateCondBr(CondV, BodyBB, FiniBB);
  CondBB = builder.GetInsertBlock();
  // Emit body value
  builder.SetInsertPoint(BodyBB);
  llvm::Value* BodyV = body->condegen(builder, varTable, context, module);
  if (!BodyV) {
    std::cout << "Error in body\n";
    return nullptr;
  }
  builder.CreateBr(CondBB);
  BodyBB = builder.GetInsertBlock();
  // Vica: What should I return?
  return builder.SetInsertPoint(FiniBB);
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
  llvm::Function* func = module->getFunction(proto->getName());

  if (!func)
    func = proto->codegen(builder, varTable, context, module);

  if (!func)
    return nullptr;

  // Create a new basic block to start insertion into.
  llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", func);
  builder.SetInsertPoint(BB);

  // Record the function arguments in the NamedValues map.
  // Vica: I dont know whether should I clear it
  // varTable.clear();
  for (auto &Arg : func->args())
    varTable[Arg.getName()] = &arg;

  if (llvm::Value* retVal = Body->codegen(builder, varTable, context, module)) {
    // Finish off the function.
    Builder.CreateRet(retVal);

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*func);

    return func;
  }

  // Error reading body, remove function.
  func->eraseFromParent();
  return nullptr;
}
