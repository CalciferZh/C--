#include "AST.h"
#include "../lexer/token.h"

// Vica: this is so dirty...
static llvm::BasicBlock* breakWhile = nullptr;

llvm::Type* getLLVMType(int tok_type, llvm::LLVMContext& context) {
  llvm::Type* type;
  switch(tok_type) {
    case tok_intType:
      type = llvm::Type::getInt32Ty(context);        
      break;
    case tok_doubleType:
      type = llvm::Type::getDoubleTy(context);
      break;
    case tok_charType:
    case tok_stringType:
      type = llvm::Type::getInt8Ty(context);
      break;
    default:
      type = llvm::Type::getInt32PtrTy(context);
      break;
  }
  return type;
}

int getTokType(llvm::Type* type) {
  if (type->isDoubleTy())
    return tok_doubleType;
  if (type->isIntegerTy(8))
    return tok_charType;
  if (type->isIntegerTy(32))
    return tok_intType;  
  return 0;
}

llvm::Value* RealExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: RealExpr\n";
  return llvm::ConstantFP::get(context, llvm::APFloat(val));
}

llvm::Value* IntExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: IntExpr\n";
  return llvm::ConstantInt::get(context, llvm::APInt(32, val, true));
}

llvm::Value* StringExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: StringExpr\n";
  return builder.CreateGlobalStringPtr(val);
}

llvm::Value* CharExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: CharExpr\n";
  return llvm::ConstantInt::get(context, llvm::APInt(8, val, true));
}

llvm::Value* VariableExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: VariableExpr\n";
  if (offset == nullptr){
    return builder.CreateLoad(varTable[name]->addr, name.c_str());
  } else {
    llvm::Value* len = offset->codegen(builder, varTable, context, module);
    llvm::Value* indexList[2] = {llvm::ConstantInt::get(len->getType(), 0), len};
    llvm::Value* ptr = builder.CreateInBoundsGEP(varTable[name]->addr, indexList);
    return builder.CreateLoad(ptr, name.c_str());
  }
}

llvm::Value* BinaryExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: BinaryExpr\n";
  if (op == tok_assignOp) { // assign is special 
    if (LHS->getClassType() != 5) {
      std::cout << "lvalue should be variable\n";
      return nullptr;
    }
    VariableExprAST* lhs = static_cast<VariableExprAST*>(LHS.get());
    llvm::Value* L = nullptr;
    if (lhs->offset == nullptr)
      L = varTable[lhs->name]->addr;
    else {
      llvm::Value* len = lhs->offset->codegen(builder, varTable, context, module);
      llvm::Value* indexList[2] = {llvm::ConstantInt::get(len->getType(), 0), len};
      L = builder.CreateInBoundsGEP(varTable[lhs->name]->addr, indexList);
    }
    llvm::Value* R = RHS->codegen(builder, varTable, context, module);
    llvm::Type* lType = L->getType();
    if (lType->isPointerTy())
      lType = lType->getContainedType(0);
    if (lType->isDoubleTy()  && !R->getType()->isDoubleTy()) {
      R = builder.CreateSIToFP(R, llvm::Type::getDoubleTy(context));
    }
    if (lType->isIntegerTy() && R->getType()->isDoubleTy()) {
      R = builder.CreateFPToSI(R, llvm::Type::getInt32Ty(context));
    }
    std::cout<<"lhs: " << lType->getTypeID() << " rhs: " << R->getType()->getTypeID() << std::endl;
    // vica: need change type maybe
    return builder.CreateStore(R, L, false);
  }
  llvm::Value* L = LHS->codegen(builder, varTable, context, module);
  llvm::Value* R = RHS->codegen(builder, varTable, context, module);
  if (!L || !R)
    return nullptr;
  bool isFloat = L->getType()->isDoubleTy() || L->getType()->isDoubleTy();
  if (op == tok_logicAndOp || op == tok_logicOrOp) { // change to UI int1
    if (L->getType()->isDoubleTy()) {
      L = builder.CreateFPToUI(L, llvm::Type::getInt1Ty(context));
    }
    if (R->getType()->isDoubleTy()) {
      R = builder.CreateFPToUI(L, llvm::Type::getInt1Ty(context));
    }
    if (!L->getType()->isIntegerTy(1)) {
      L = builder.CreateSExt(L, llvm::Type::getInt1Ty(context));
    }
    if (!R->getType()->isIntegerTy(1)) {
      R = builder.CreateSExt(R, llvm::Type::getInt1Ty(context));
    }
  } else { 
    if (isFloat) { // is float, change to double both
      if (!L->getType()->isDoubleTy()) {
        L = builder.CreateSIToFP(L, llvm::Type::getDoubleTy(context));
      }
      if (!R->getType()->isDoubleTy()) {
        R = builder.CreateSIToFP(R, llvm::Type::getDoubleTy(context));
      }
    } else { // not float, change to i32 both
      if (!L->getType()->isIntegerTy(32)) {
        L = builder.CreateSExt(L, llvm::Type::getInt32Ty(context));
      }
      if (!R->getType()->isIntegerTy(32)) {
        R = builder.CreateSExt(R, llvm::Type::getInt32Ty(context));
      }
    }
  }
  switch (op) {
  case tok_addOp:
    return isFloat ? builder.CreateFAdd(L, R, "addtmp") : builder.CreateAdd(L, R, "addtmp");
  case tok_subtractOp:
    return isFloat ? builder.CreateFSub(L, R, "subtmp") : builder.CreateSub(L, R, "addtmp");
  case tok_multiplyOp:
    return isFloat ? builder.CreateFMul(L, R, "multmp") : builder.CreateMul(L, R, "addtmp");
  case tok_divideOp:
    return isFloat ? builder.CreateFDiv(L, R, "divtmp") : builder.CreateSDiv(L, R, "addtmp");
  case tok_lessOp:
    return isFloat ? builder.CreateFCmpULT(L, R, "cmptmp") : builder.CreateICmpSLT(L, R, "cmptmp");
  case tok_greaterOp:
    return isFloat ? builder.CreateFCmpUGT(L, R, "cmptmp") : builder.CreateICmpSGT(L, R, "cmptmp");
  case tok_euqalOp:
    return isFloat ? builder.CreateFCmpUEQ(L, R, "eqtmp") : builder.CreateICmpEQ(L, R, "eqtmp");
  case tok_nEqualOp:
    return isFloat ? builder.CreateFCmpUNE(L, R, "neqtmp") : builder.CreateICmpNE(L, R, "neqtmp");
  case tok_modOp:
    return isFloat ? builder.CreateFRem(L, R, "remtmp") : builder.CreateSRem(L, R, "remtmp");
  case tok_logicOrOp:
    return builder.CreateOr(L, R, "ortmp");
  case tok_logicAndOp:
    return builder.CreateAnd(L, R, "andtmp");
  default:
    std::cout << "unknown binary operator\n";
    return nullptr;
  }
}

llvm::Value* DeclareExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: DeclareExpr\n";
  if (varTable.find(name) != varTable.end()) {
    std::cout << "Variable redefinition\n";
    return nullptr;
  } else {
    llvm::Type* type = getLLVMType(tp, context);
    if (size < 0) {
      std::cout << "Negative size\n";
      return nullptr;
    }
    if (size != 0) {
      type = llvm::ArrayType::get(type, size);
    }
    llvm::AllocaInst* addr = builder.CreateAlloca(type, 0, nullptr, name.c_str());
    varTable[name] = std::unique_ptr<Variable>(new Variable(name, tp, addr));
  }
  if (init) {
    llvm::Value* R = init->codegen(builder, varTable, context, module);
    // Vica: Since we have create globle string, now we gonna try to figure out how to extern 'puts'
    if (!R && init->getClassType() != 3) { // Vica: R==nullptr is acceptable only if stringexpr
      std::cout << "Invalid expression\n";
      return nullptr;
    }
    if (size != 0) {
      if (init->getClassType() == 3) {
        // Vica: Both dynamic_cast and llvm::dyn_cast fail.. wtf
        StringExprAST* str = static_cast<StringExprAST*>(init.get());
        if (str) {
          if (unsigned(size) < str->val.length()) {
            std::cout << "Too long\n";
            return nullptr;
          }
          unsigned int idx = 0;
          for (auto c : str->val) {
            llvm::Value* loc = builder.CreateInBoundsGEP(varTable[name]->addr, llvm::ConstantInt::get(context, llvm::APInt(32, idx, true)));
            builder.CreateStore(llvm::ConstantInt::get(context, llvm::APInt(8, int(c), true)), loc);
            idx++;
          }
        }
      }
      // Vica: how to init an array? 
    } else {
      builder.CreateStore(R, varTable[name]->addr);
    }
    return R;
  } else {
    return nullptr;
  }
}

llvm::Value* IfExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: IfExpr\n";
  llvm::Value* CondV = cond->codegen(builder, varTable, context, module);
  if (!CondV) {
    std::cout << "Error in condition\n";
    return nullptr;
  }
  CondV = builder.CreateFCmpONE(CondV, llvm::ConstantFP::get(context, llvm::APFloat(0.0)), "ifcond");
  // Vica: why use function here?
  llvm::Function* func = builder.GetInsertBlock()->getParent();
  // Create blocks for the then and else cases.  Insert the 'then' block at the end of the function.
  llvm::BasicBlock* ThenBB = llvm::BasicBlock::Create(context, "then", func);
  llvm::BasicBlock* ElseBB = llvm::BasicBlock::Create(context, "else", func);
  llvm::BasicBlock* MergeBB = llvm::BasicBlock::Create(context, "ifcont", func);
  builder.CreateCondBr(CondV, ThenBB, ElseBB);
  // Emit then value.
  builder.SetInsertPoint(ThenBB);
  bool firstThen = true, firstElse = true;
  llvm::Value* firstThenV = nullptr;
  llvm::Value* firstElseV = nullptr;
  for (auto& then : ifBody) {
    llvm::Value* ThenV = then->codegen(builder, varTable, context, module);
    if (firstThen) {
      firstThenV = ThenV;
      firstThen = false;
    }
    if (!ThenV) {
      std::cout << "Error in then\n";
      return nullptr;
    }
  }
  builder.CreateBr(MergeBB);
  // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
  ThenBB = builder.GetInsertBlock();
  // Emit else block.
  //func->getBasicBlockList().push_back(ElseBB);
  builder.SetInsertPoint(ElseBB);
  for (auto& elsse : elseBody) {
    llvm::Value *ElseV = elsse->codegen(builder, varTable, context, module);
    if (firstElse) {
      firstElseV = ElseV;
      firstElse = false;
    }
    if (!ElseV) {
      std::cout << "Error in else\n";
      return nullptr;
    }
  }
  builder.CreateBr(MergeBB);
  // codegen of 'Else' can change the current block, update ElseBB for the PHI.
  ElseBB = builder.GetInsertBlock();
  // Emit merge block.
  //func->getBasicBlockList().push_back(MergeBB);
  builder.SetInsertPoint(MergeBB);
  llvm::PHINode *PN = builder.CreatePHI(llvm::Type::getDoubleTy(context), 2, "iftmp");
  PN->addIncoming(firstThenV, ThenBB);
  PN->addIncoming(firstElseV, ElseBB);
  return PN;
}

llvm::Value* WhileExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: WhileExpr\n";
  llvm::Function* func = builder.GetInsertBlock()->getParent();
  llvm::BasicBlock* CondBB = llvm::BasicBlock::Create(context, "whilecond", func);
  llvm::BasicBlock* BodyBB = llvm::BasicBlock::Create(context, "whilebody", func);
  llvm::BasicBlock* FiniBB = llvm::BasicBlock::Create(context, "whilefinish", func);
  breakWhile = FiniBB;
  builder.CreateBr(CondBB);
  // Emit cond value
  builder.SetInsertPoint(CondBB);
  llvm::Value* CondV = cond->codegen(builder, varTable, context, module);
  if (!CondV) {
    std::cout << "Error in condition\n";
    return nullptr;
  }
  CondV = builder.CreateFCmpONE(CondV, llvm::ConstantFP::get(context, llvm::APFloat(0.0)), "whilecondtmp");
  builder.CreateCondBr(CondV, BodyBB, FiniBB);
  CondBB = builder.GetInsertBlock();
  // Emit body value
  builder.SetInsertPoint(BodyBB);
  for (auto& b : body) {
    llvm::Value* BodyV = b->codegen(builder, varTable, context, module);
    if (!BodyV) {
        std::cout << "Error in body\n";
        return nullptr;
      }
  }
  builder.CreateBr(CondBB);
  BodyBB = builder.GetInsertBlock();
  builder.SetInsertPoint(FiniBB);
  breakWhile = nullptr;
  // Vica: What should I return?
  return nullptr;
}

llvm::Value* ReturnExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: ReturnExpr\n";
  return builder.CreateRet(retExpr->codegen(builder, varTable, context, module));
}

llvm::Value* CallExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: CallExpr\n";
  // Vica: need to get the func
  llvm::Function* func = module->getFunction(callee);
  if (!func) {
    std::cout << "Unknown function\n";
    return nullptr;
  }
  if (params.size() != func->arg_size()) {
    std::cout << "Unmatch arguments\n";
    return nullptr;
  }
  std::vector<llvm::Value*> Params;
  for (auto& param : params) {
    llvm::Value* v = param->codegen(builder, varTable, context, module);
    Params.push_back(v);
  }
  return builder.CreateCall(func, Params, callee);
}

llvm::Value* BreakExprAST::codegen(CODEGENPARM) {
  std::cout << "Generating: BreakExpr\n";
  if (breakWhile) {
    return builder.CreateBr(breakWhile);
  } else {
    std::cout << "No loop\n";
    return nullptr;
  }
}

llvm::Function* PrototypeAST::codegen(CODEGENPARM) {
  std::cout << "Generating: Prototype\n";
  // Make the function type:  double(double,double) etc.

  std::vector<llvm::Type*> types;
  for (auto& arg : args) {
    llvm::Type* type = getLLVMType(arg->tp, context);
    types.push_back(type);
  }
  llvm::Type* ret = getLLVMType(retType, context);
  llvm::FunctionType* FT = llvm::FunctionType::get(ret, types, false);
  llvm::Function* F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, module.get());

  // Set names for all arguments.
  unsigned Idx = 0;
  for (auto& arg : F->args())
    arg.setName(args[Idx++]->name);
  return F;
}

llvm::Function* FunctionAST::codegen(CODEGENPARM) {
  std::cout << "Generating: Function\n";
  // First, check for an existing function from a previous 'extern' declaration.
  llvm::Function* func = module->getFunction(proto->name);

  if (!func)
    func = proto->codegen(builder, varTable, context, module);

  if (!func)
    return nullptr;
  if (body.empty()) { // just a declaration
    return func;
  }
  // Create a new basic block to start insertion into.
  llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", func);
  builder.SetInsertPoint(BB);
  // Record the function arguments in the varTable map.

  // varTable.clear();
  // Vica: I dont know whether should I clear it?
  // Vica: I dont know how to use it. Arg is llvm::Argument type while varTable is llvm::AllocaInst.
  std::map<std::string, std::unique_ptr<Variable>> selfVarTable;
  //
  for (auto& Arg : func->args())
    //NamedValues[Arg.getName()] = &Arg;
    selfVarTable[Arg.getName()] = std::unique_ptr<Variable>(new Variable(Arg.getName(), getTokType(Arg.getType()), nullptr));
  for (auto& b : body) {
    b->codegen(builder, selfVarTable, context, module);
  }
  return func;
  // Vica: It tell me there is some bug and delete it wtf?
  // Notice: document say verifyFunction return false when no erros
  /*if (llvm::verifyFunction(*func) == false) {
    return func;
  } else {
    func->eraseFromParent();
    return nullptr;
  }*/
  /*
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
  */
}
