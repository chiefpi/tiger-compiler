#include "node.h"
#include "codeGen.h"
#include "parser.hpp"


/* Compile the AST into a module */
void CodeGenContext::generateCode(NExpr &root) {
#ifdef DEBUG
    std::cout << "Generating code...\n";
#endif
    /* Create the top level interpreter function to call as entry */
    std::vector<Type*> argTypes;
    FunctionType *ftype = FunctionType::get(Type::getVoidTy(MyContext), makeArrayRef(argTypes), false);
    mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
    BasicBlock *bblock = BasicBlock::Create(MyContext, "entry", mainFunction, 0);

    /* Push a new variable/block context */
    pushBlock(bblock);
    root.codeGen(*this); /* emit bytecode for the toplevel block */
    ReturnInst::Create(MyContext, bblock);
    popBlock();

#ifdef DEBUG
    std::cout << "Code is generated.\n";
#endif
    /* Print the bytecode in a human-readable format */
    // module->dump();

    legacy::PassManager pm;

    /* Optimizations */
	pm.add(createBasicAliasAnalysisPass());
	pm.add(createPromoteMemoryToRegisterPass());
	pm.add(createInstructionCombiningPass());
	pm.add(createReassociatePass());
	pm.add(createGVNPass());
	pm.add(createCFGSimplificationPass());

    pm.add(createPrintModulePass(outs()));
    pm.run(*module);
}

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode() {
#ifdef DEBUG
    std::cout << "Running code...\n";
#endif
    ExecutionEngine *ee = EngineBuilder(unique_ptr<Module>(module)).create();
    ee->finalizeObject();
    std::vector<GenericValue> noargs;
    GenericValue v = ee->runFunction(mainFunction, noargs);
#ifdef DEBUG
    std::cout << "Code was run.\n";
#endif
    return v;
}

/* Returns an LLVM type based on the symbol */
Type *CodeGenContext::typeOf(const Symbol &sb) {
    return types[sb];
}


/* Code Generation */

// Value *NExpr::codeGen(CodeGenContext &context) {}

Value *NExprList::codeGen(CodeGenContext &context) {

}

// Value *NDecl::codeGen(CodeGenContext &context) {}

Value *NDeclList::codeGen(CodeGenContext &context) {

}

// Value *NVar::codeGen(CodeGenContext &context) {}

Value *NVarList::codeGen(CodeGenContext &context) {

}

// Value *NType::codeGen(CodeGenContext &context) {}

Value *NFieldTypeList::codeGen(CodeGenContext &context) {

}

Value *NFieldExprList::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating field expression list " << value << std::endl;
#endif
    for (NFieldExprList *it = )
    return ->codeGen(context);
}

Value *NStrExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating string " << value << std::endl;
#endif
    return builder.CreateGlobalStringPtr(value, "str");
}

Value *NIntExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating integer " << value << std::endl;
#endif
    // return ConstantInt::get(Type::getInt64Ty(MyContext), value, true);
    return ConstantInt::get(module->getContext(), APInt(64, value), true);
}

Value *NNilExpr::codeGen(CodeGenContext &context) { // TODO: verify null pointer
#ifdef DEBUG
    std::cout << "Creating nil" << std::endl;
#endif
    return ConstantPointerNull::get(PointerType::getUnqual(StructType::get(context)));
}

Value *NOpExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating binary operation " << op << std::endl;
#endif
    Instruction::BinaryOps inst;
    switch (op) { // TODO: check token consistency
        case PLUS: inst = Instruction::Add; goto math;
        case MINUS: inst = Instruction::Sub; goto math;
        case MUL: inst = Instruction::Mul; goto math;
        case DIV: inst = Instruction::SDiv; goto math;
        case AND: inst = Instruction::And; goto math;
        case OR: inst = Instruction::Or; goto math;
        case XOR: inst = Instruction::Xor; goto math;
        case EQ: inst = ICmpInst::ICMP_SEQ; goto cmp; 
        case NE: inst = ICmpInst::ICMP_SNE; goto cmp; 
        case LT: inst = ICmpInst::ICMP_SLT; goto cmp; 
        case LE: inst = ICmpInst::ICMP_SLE; goto cmp; 
        case GT: inst = ICmpInst::ICMP_SGT; goto cmp; 
        case GE: inst = ICmpInst::ICMP_SGE; goto cmp; 
    }
    return NULL;
math:
    return BinaryOperator::Create(inst, lhs.codeGen(context), 
        rhs.codeGen(context), "", context.currentBlock());
cmp:
    return new ZExtInst( // compare and cast
        new ICmpInst(*context.currentBlock(), inst, lhs.codeGen(context), rhs.codeGen(context), ""),
        IntegerType::get(MyContext, 64), "", context.currentBlock());
}

Value *NAssignExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating assignment for " << lhs.name << std::endl;
#endif
    vars.find()
    if (context.locals().find(lhs.name) == context.locals().end()) {
        std::cerr << "Undeclared variable " << lhs.name << std::endl;
        return NULL;
    }
    return new StoreInst(rhs.codeGen(context), context.locals()[lhs.name], false, context.currentBlock());
}

Value *NRecordExpr::codeGen(CodeGenContext &context) { // TODO
#ifdef DEBUG
    std::cout << "Creating record for " << lhs.name << std::endl;
#endif
    auto etype = cast<PointerType>(typeOf(type))->getElementType();
    auto size = context.module->getDataLayout().getTypeAllocSize(etype);
    Value *var = builder.CreateCall(
        createIntrinsicFunction(
            "",
            {Type::getInt64Ty(context)},
            Type::getInt8PtrTy(context))
        ConstantInt::get(context.intType, APInt(64, size)),
        "");
    var = builder.CreateBitCast(var, type, "");
    size_t idx = 0;
    for (NFieldExprList *it = fields; it != NULL; it = it->next) {
        auto exp = (*it)->codeGen(context);
        auto eptr = builder.CreateGEP(
            (*it)->type,
            var,
            ConstantInt::get(Type::getInt64Ty(MyContext), APInt(64, idx)),
            "");
        // context.checkStore(exp, eptr);
        builder.CreateStore(exp, eptr);
        ++idx;
    }
    return var;
}

Value *NArrayExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating array" << std::endl;
#endif
    ArrayType* arrayTy = ArrayType::get(IntegerType::get(MyContext, 64), size.codeGen(context)); // TODO: int to type
    return  new AllocaInst(arrayTy, "", context.currentBlock());
}

Value *NCallExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating function call " << id.name << std::endl;
#endif
    Function *function = context.module->getFunction(id.name.c_str());
    if (function == NULL)
        std::cerr << "No such function: " << id.name << std::endl;

    std::vector<Value*> args;
    for (NExprList *it = params; it != NULL; it = it->next)
        args.push_back((*it).codeGen(context));

    return CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());;
}

Value *NSeqExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating sequential expression " << id.name << std::endl;
#endif
    Value *last = NULL;
    for (NExprList *it = exprs; it != NULL; it = it->next)
        last = (*it)->codeGen(context);
    return last;
}

Value *NIfExpr::codeGen(CodeGenContext &context) { // TODO: verify
#ifdef DEBUG
    std::cout << "Creating if expression" << std::endl;
#endif
    test = builder.CreateICmpNE(test->codeGen(context), ConstantInt::get(intType, APInt(64, 0)), "");
    auto function = builder.GetInsertBlock()->getParent();

    auto thenBlock = BasicBlock::Create(MyContext, "", function);
    auto elseBlock = BasicBlock::Create(MyContext, "");
    auto mergeBlock = BasicBlock::Create(MyContext, "");

    builder.CreateCondBr(test, thenBlock, elseBlock);
    builder.SetInsertPoint(thenBlock);

    auto then = thenClause->codeGen(context);
    builder.CreateBr(mergeBlock);

    thenBlock = builder.GetInsertBlock();
    function->getBasicBlockList().push_back(elseBlock);
    builder.SetInsertPoint(elseBlock);

    Value *elseValue;
    if (elseClause) {
        elseValue = elseClause->codeGen(context);
        if (!elseValue)
            return NULL;
    }

    builder.CreateBr(mergeBlock);
    elseBlock = builder.GetInsertBlock();

    function->getBasicBlockList().push_back(mergeBlock);
    builder.SetInsertPoint(mergeBlock);

    if (elseClause && !then->getType()->isVoidTy() && !elseValue->getType()->isVoidTy()) {
        auto pn = builder.CreatePHI(then->getType(), 2, "");
        // then = context.convertNil(then, elseValue);
        // elseValue = context.convertNil(elseValue, then);
        pn->addIncoming(then, thenBlock);
        pn->addIncoming(elseValue, elseBlock);
        return pn;
    }

    return Constant::getNullValue(Type::getInt64Ty(MyContext));
}

Value *NWhileExpr::codeGen(CodeGenContext &context) {
    auto function = builder.GetInsertBlock()->getParent();
    auto testBlock = BasicBlock::Create(MyContext, "", function);
    auto loopBlock = BasicBlock::Create(MyContext, "", function);
    auto nextBlock = BasicBlock::Create(MyContext, "", function);
    auto afterBlock = BasicBlock::Create(MyContext, "", function);

    builder.CreateBr(testBlock);
    builder.SetInsertPoint(testBlock);

    builder.CreateCondBr( // to after or loop
        builder.CreateICmpEQ(test->codeGen(context), context.zero, ""),
        afterBlock,
        loopBlock);
    builder.SetInsertPoint(loopBlock);

    body->codeGen(context);

    builder.CreateBr(nextBlock);
    builder.SetInsertPoint(nextBlock);
    builder.CreateBr(testBlock);

    builder.SetInsertPoint(afterBlock);

    return Constant::getNullValue(Type::getInt64Ty(MyContext));
}

Value *NForExpr::codeGen(CodeGenContext &context) {
    Value *low = id->codeGen(context);
    Value *high = high->codeGen(context);
    Function *function = builder.GetInsertBlock()->getParent(); // TODO

    builder.CreateStore(low, varDec_->read(context));

    BasicBlock *testBlock = BasicBlock::Create(MyContext, "", function);
    BasicBlock *loopBlock = BasicBlock::Create(MyContext, "", function);
    BasicBlock *nextBlock = BasicBlock::Create(MyContext, "", function);
    BasicBlock *afterBlock = BasicBlock::Create(MyContext, "", function);

    builder.CreateBr(testBlock);
    builder.SetInsertPoint(testBlock);

    auto cond = builder.CreateICmpSLE(
        builder.CreateLoad(varDec_->read(context), var_), high, "");

    builder.CreateCondBr(cond, loopBlock, afterBlock);
    builder.SetInsertPoint(loopBlock);

    auto oldVal = context.valueDecs[var_];
    if (oldVal) context.valueDecs.popOne(var_);
    context.valueDecs.push(var_, varDec_);
    // TODO: check non-type value
    if (!body_->codeGen(context)) return NULL;

    builder.CreateBr(nextBlock);
    builder.SetInsertPoint(nextBlock);

    auto nextVar = builder.CreateAdd(
        builder.CreateLoad(varDec_->read(context), var_),
        ConstantInt::get(MyContext, APInt(64, 1)), "");
    builder.CreateStore(nextVar, varDec_->read(context));

    builder.CreateBr(testBlock);
    builder.SetInsertPoint(afterBlock);

    if (oldVal)
        context.valueDecs[var_] = oldVal;
    else
        context.valueDecs.popOne(var_);

    return Constant::getNullValue(Type::getInt64Ty(MyContext));
}

Value *NBreakExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating break" << std::endl;
#endif
    context.builder.CreateBr(std::get<1>(context.loopStack.top()));
    return Constant::getNullValue(Type::getInt64Ty(MyContext)); // nothing
}

Value *NLetExpr::codeGen(CodeGenContext &context) {
    context.tenv.enterScope();
    context.vars.enterScope();
    for (auto &dec : decs_) dec->codeGen(context);
    auto result = body_->codeGen(context);
    context.vars.quitScope();
    context.types.quitScope();
    return result;
}

Value *NFuncDecl::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating function: " << id.name << std::endl;
#endif
    std::vector<Type*> argTypes;
    for (NFieldTypeList *it = params; it != NULL; it = it.next)
        argTypes.push_back(typeOf(it.type));

    FunctionType *ftype = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
    Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
    BasicBlock *bblock = BasicBlock::Create(MyContext, "entry", function, 0);

    context.pushBlock(bblock);

    Function::arg_iterator argsValues = function->arg_begin();
    Value* argumentValue;

    for (NFieldTypeList *it = params; it != NULL; it = it->next) {
        (*it).codeGen(context);

        argumentValue = &*argsValues++;
        argumentValue->setName((*it)->id.name.c_str());
        StoreInst *inst = new StoreInst(argumentValue, context.locals()[(*it)->id.name], false, bblock);
    }
    
    block.codeGen(context);
    ReturnInst::Create(MyContext, context.getCurrentReturnValue(), bblock);

    context.popBlock();

    return function;
}

Value *NTypeDecl::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating type declaration " << type.name << " " << id.name << std::endl;
#endif

}

Value *NVarDecl::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating variable declaration " << type.name << " " << id.name << std::endl;
#endif
    AllocaInst *alloc = new AllocaInst(typeOf(type), "", context.currentBlock());
    venv.push(id, alloc); // TODO: modify env
    if (initValue != NULL) {
        NAssignExpr assn(line, index, id, *initValue);
        assn.codeGen(context);
    }
    return alloc;
  auto init = initValue->codegen(context);
  if (!init) return nullptr;
  auto var = context.checkStore(init, read(context));
  // context.builder.CreateStore(init, variable);
  context.valueDecs.push(name_, this);
  return var;
}

Value *NArrayType::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating array type " << type.name <<
#endif
}

Value *NRecordType::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating record type " << type.name <<
#endif
}

Value *NNameType::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating name type " << *id << std::endl;
#endif
}

Value *NSimpleVar::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating simple variable " << id.name << std::endl;
#endif
    // return context.valueDecs[name_]->read(context);
    return context.venv[name];
}

Value *NFieldVar::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating field variable " << var.name << " " << id.name << std::endl;
#endif
    return builder.CreateGEP(
        type, // TODO
        builder.CreateLoad(var->codeGen(context), ""),
        ConstantInt::get(Type::getInt64Ty(MyContext), APInt(64, id)),
        "");
}

Value *NSubscriptVar::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating subscription variable " << var.name << " " << id.name << std::endl;
#endif
    return builder.CreateGEP(
        type, // TODO
        builder.CreateLoad(var->codeGen(context), ""),
        sub->codeGen(context),
        "");
}


Value* Symbol::codeGen(CodeGenContext& context) {
#ifdef DEBUG
    std::cout << "Creating identifier reference " << name << std::endl;
#endif
    if (context.locals().find(name) == context.locals().end()) {
        std::cerr << "Undeclared variable: " << name << std::endl;
        return NULL;
    } // TODO
    return new LoadInst(context.locals()[name], "", false, context.currentBlock());
}







// Value* NBlock::codeGen(CodeGenContext& context)
// {
//     StatementList::const_iterator it;
//     Value *last = NULL;
//     for (it = statements.begin(); it != statements.end(); it++) {
// #ifdef DEBUG
//         std::cout << "Generating code for " << typeid(**it).name() << std::endl;
// #endif
//         last = (**it).codeGen(context);
//     }
// #ifdef DEBUG
//     std::cout << "Creating block" << std::endl;
// #endif
//     return last;
// }

// Value* NExpressionStatement::codeGen(CodeGenContext& context)
// {
// #ifdef DEBUG
//     std::cout << "Generating code for " << typeid(expression).name() << std::endl;
// #endif
//     return expression.codeGen(context);
// }

// Value* NReturnStatement::codeGen(CodeGenContext& context)
// {
// #ifdef DEBUG
//     std::cout << "Generating return code for " << typeid(expression).name() << std::endl;
// #endif
//     Value *returnValue = expression.codeGen(context);
//     context.setCurrentReturnValue(returnValue);
//     return returnValue;
// }

// Value* NExternDeclaration::codeGen(CodeGenContext& context)
// {
//     std::vector<Type*> argTypes;
//     NFieldTypeList::const_iterator it;
//     for (it = params.begin(); it != params.end(); it++) {
//         argTypes.push_back(typeOf((**it).type));
//     }
//     FunctionType *ftype = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
//     Function *function = Function::Create(ftype, GlobalValue::ExternalLinkage, id.name.c_str(), context.module);
//     return function;
// }


