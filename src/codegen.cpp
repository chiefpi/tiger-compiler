#include "node.h"
#include "codeGen.h"
#include "parser.hpp"


/* Compile the AST into a module */
void CodeGenContext::generateCode(Node &root) {
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
    /* Print the bytecode in a human-readable format 
       to see if our program compiled properly
     */
    // module->dump();

    legacy::PassManager pm;
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

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const Symbol& type) {
    if (type.name.compare("int") == 0)
        return Type::getInt64Ty(MyContext);
    else if (type.name.compare("string") == 0)
        return Type::getDoubleTy(MyContext); // TODO: string
    return Type::getVoidTy(MyContext);
}


/* Code Generation */

Value *NExpr::codeGen(CodeGenContext &context) {
    
}

Value *NExprList::codeGen(CodeGenContext &context) {

}

Value *NDecl::codeGen(CodeGenContext &context) {

}

Value *NDeclList::codeGen(CodeGenContext &context) {

}

Value *NVar::codeGen(CodeGenContext &context) {

}

Value *NVarList::codeGen(CodeGenContext &context) {

}

Value *NType::codeGen(CodeGenContext &context) {

}

Value *NFieldTypeList::codeGen(CodeGenContext &context) {

}

Value *NFieldExprList::codeGen(CodeGenContext &context) {
    return exp->codeGen(context);
}

Value *NStrExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating string " << value << endl;
#endif
    return builder.CreateGlobalStringPtr(value, "str");
}

Value *NIntExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating integer " << value << endl;
#endif
    // return ConstantInt::get(Type::getInt64Ty(MyContext), value, true);
    return ConstantInt::get(module->getContext(), APInt(64, value), true);
}

Value *NNilExpr::codeGen(CodeGenContext &context) { // TODO: verify null pointer
#ifdef DEBUG
    std::cout << "Creating nil" << endl;
#endif
    return ConstantPointerNull::get(PointerType::getUnqual(StructType::get(context)));
}

Value *NOpExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating binary operation " << op << endl;
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
    return new ZExtInst(
        new ICmpInst(*context.currentBlock(), inst, int32_11, const_int32_3, ""),
        IntegerType::get(mod->getContext(), 32), "", context.currentBlock());
}

Value *NAssignExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating assignment for " << lhs.name << endl;
#endif
    if (context.locals().find(lhs.name) == context.locals().end()) {
        std::cerr << "Undeclared variable " << lhs.name << endl;
        return NULL;
    }
    return new StoreInst(rhs.codeGen(context), context.locals()[lhs.name], false, context.currentBlock());
}

Value *NRecordExpr::codeGen(CodeGenContext &context) { // TODO
#ifdef DEBUG
    std::cout << "Creating record for " << lhs.name << endl;
#endif
    builder.GetInsertBlock()->getParent();
    // auto var = createEntryBlockAlloca(function, type, "record");
    auto eleType = context.getElementType(type_);
    auto size = context.module->getDataLayout().getTypeAllocSize(eleType);
    Value *var = builder.CreateCall(
        context.allocaRecordFunction,
        ConstantInt::get(context.intType, APInt(64, size)),
        "alloca");
    var = builder.CreateBitCast(var, type_, "record");
    size_t idx = 0;
    for (auto &field : fieldExps_) {
        auto exp = field->codeGen(context);
        auto elementPtr = builder.CreateGEP(
            field->type_, var,
            ConstantInt::get(Type::getInt64Ty(MyContext), APInt(64, idx)),
            "");
        context.checkStore(exp, elementPtr);
        // builder.CreateStore(exp, elementPtr);
        ++idx;
    }
    return var;
}

Value *NArrayExpr::codeGen(CodeGenContext &context) {
    auto function = builder.GetInsertBlock()->getParent();
    // if (!type_->isPointerTy()) return context.logErrorV("Array type required");
    auto eleType = context.getElementType(type_);
    auto size = size_->codeGen(context);
    auto init = init_->codeGen(context);
    auto eleSize = context.module->getDataLayout().getTypeAllocSize(eleType);
    Value *arrayPtr = builder.CreateCall(
        context.allocaArrayFunction,
        std::vector<Value *>{
            size, ConstantInt::get(Type::getInt64Ty(MyContext),
                                        APInt(64, eleSize))},
        "alloca");
    arrayPtr = builder.CreateBitCast(arrayPtr, type_, "array");

    // auto arrayPtr = createEntryBlockAlloca(function, eleType, "arrayPtr",
    // size);
    auto zero = ConstantInt::get(MyContext, APInt(64, 0, true));

    std::string indexName = "index";
    auto indexPtr = context.createEntryBlockAlloca(
        function, Type::getInt64Ty(MyContext), indexName);
    // before loop:
    builder.CreateStore(zero, indexPtr);

    auto testBlock = BasicBlock::Create(MyContext, "test", function);
    auto loopBlock = BasicBlock::Create(MyContext, "loop", function);
    auto nextBlock = BasicBlock::Create(MyContext, "next", function);
    auto afterBlock = BasicBlock::Create(MyContext, "after", function);

    builder.CreateBr(testBlock);

    builder.SetInsertPoint(testBlock);

    auto index = builder.CreateLoad(indexPtr, indexName);
    auto cond = builder.CreateICmpSLT(index, size, "loopcond");
    // auto loopEndBB = builder.GetInsertBlock();

    // goto after or loop
    builder.CreateCondBr(cond, loopBlock, afterBlock);

    builder.SetInsertPoint(loopBlock);

    // loop:
    // variable->addIncoming(low, preheadBB);

    // TODO: check its non-type value
    auto elePtr = builder.CreateGEP(eleType, arrayPtr, index, "elePtr");
    context.checkStore(init, elePtr);
    // builder.CreateStore(init, elePtr);
    // goto next:
    builder.CreateBr(nextBlock);

    // next:
    builder.SetInsertPoint(nextBlock);

    auto nextVar = builder.CreateAdd(
        index, ConstantInt::get(MyContext, APInt(64, 1)),
        "nextvar");
    builder.CreateStore(nextVar, indexPtr);

    builder.CreateBr(testBlock);

    // after:
    builder.SetInsertPoint(afterBlock);

    // variable->addIncoming(next, loopEndBB);

    return arrayPtr;
}

Value *NCallExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating method call: " << id.name << endl;
#endif
    Function *function = context.module->getFunction(id.name.c_str());
    if (function == NULL)
        std::cerr << "No such function: " << id.name << endl;

    std::vector<Value*> args;
    for (NExprList *it = params; it != NULL; it = it->next)
        args.push_back((*it).codeGen(context));

    return CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());;
}

Value *NSeqExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating sequential expression " << id.name << endl;
#endif
    Value *last = NULL;
    for (NExprList *it = exprs; it != NULL; it = it->next)
        last = (*it)->codeGen(context);
    return last;
}

Value *NIfExpr::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating if expression" << endl;
#endif
    auto test = test->codeGen(context);

    test = builder.CreateICmpNE(test, context.zero, "iftest");
    auto function = builder.GetInsertBlock()->getParent();

    auto thenBlock = BasicBlock::Create(MyContext, "then", function);
    auto elseBlock = BasicBlock::Create(MyContext, "else");
    auto mergeBlock = BasicBlock::Create(MyContext, "ifcont");

    builder.CreateCondBr(test, thenBlock, elseBlock);

    builder.SetInsertPoint(thenBlock);

    auto then = thenClause->codeGen(context);
    builder.CreateBr(mergeBlock);

    thenBlock = builder.GetInsertBlock();

    function->getBasicBlockList().push_back(elseBlock);
    builder.SetInsertPoint(elseBlock);

    Value *elsee;
    if (else_) {
        elsee = else_->codeGen(context);
        if (!elsee) return NULL;
    }

    builder.CreateBr(mergeBlock);
    elseBlock = builder.GetInsertBlock();

    function->getBasicBlockList().push_back(mergeBlock);
    builder.SetInsertPoint(mergeBlock);

    if (else_ && !then->getType()->isVoidTy() && !elsee->getType()->isVoidTy()) {
        auto PN = builder.CreatePHI(then->getType(), 2, "iftmp");
        then = context.convertNil(then, elsee);
        elsee = context.convertNil(elsee, then);
        PN->addIncoming(then, thenBlock);
        PN->addIncoming(elsee, elseBlock);

        return PN;
    } else {
        return Constant::getNullValue(
            Type::getInt64Ty(MyContext));
    }
}

Value *NWhileExpr::codeGen(CodeGenContext &context) {
    auto function = builder.GetInsertBlock()->getParent();
    auto testBlock = BasicBlock::Create(MyContext, "", function);
    auto loopBlock = BasicBlock::Create(MyContext, "", function);
    auto nextBlock = BasicBlock::Create(MyContext, "", function);
    auto afterBlock = BasicBlock::Create(MyContext, "", function);
    // context.loopStack.push({nextBlock, afterBlock});

    builder.CreateBr(testBlock);
    builder.SetInsertPoint(testBlock);

    builder.CreateCondBr( // after or loop
        builder.CreateICmpEQ(test->codeGen(context), context.zero, "loopcond"),
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
    // context.loopStack.push({nextBlock, afterBlock});

    builder.CreateBr(testBlock);

    builder.SetInsertPoint(testBlock);

    auto cond = builder.CreateICmpSLE(
        builder.CreateLoad(varDec_->read(context), var_), high,
        "loopcond");
    // auto loopEndBB = builder.GetInsertBlock();

    // goto after or loop
    builder.CreateCondBr(cond, loopBlock, afterBlock);

    builder.SetInsertPoint(loopBlock);

    // loop:
    // variable->addIncoming(low, preheadBB);

    auto oldVal = context.valueDecs[var_];
    if (oldVal) context.valueDecs.popOne(var_);
    context.valueDecs.push(var_, varDec_);
    // TODO: check its non-type value
    if (!body_->codeGen(context)) return NULL;

    // goto next:
    builder.CreateBr(nextBlock);

    // next:
    builder.SetInsertPoint(nextBlock);

    auto nextVar = builder.CreateAdd(
        builder.CreateLoad(varDec_->read(context), var_),
        ConstantInt::get(MyContext, APInt(64, 1)), "nextvar");
    builder.CreateStore(nextVar, varDec_->read(context));

    builder.CreateBr(testBlock);

    // after:
    builder.SetInsertPoint(afterBlock);

    // variable->addIncoming(next, loopEndBB);

    if (oldVal)
        context.valueDecs[var_] = oldVal;
    else
        context.valueDecs.popOne(var_);

    // context.loopStack.pop();

    return Constant::getNullValue(Type::getInt64Ty(MyContext));
}

Value *NBreakExpr::codeGen(CodeGenContext &context) {

    context.builder.CreateBr(std::get<1>(context.loopStack.top()));
    return llvm::Constant::getNullValue(llvm::Type::getInt64Ty(MyContext)); // nothing
}

Value *NLetExpr::codeGen(CodeGenContext &context) {
    context.valueDecs.enter();
    context.functionDecs.enter();
    for (auto &dec : decs_) dec->codeGen(context);
    auto result = body_->codeGen(context);
    context.functionDecs.exit();
    context.valueDecs.exit();
    return result;
}

Value *NFuncDecl::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating function: " << id.name << endl;
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

    for (NFieldTypeList &it = params; it != NULL; it = it.next) {
        it.codeGen(context);
        
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
    std::cout << "Creating type declaration " << type.name << " " << id.name << endl;
#endif

}

Value *NVarDecl::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating variable declaration " << type.name << " " << id.name << endl;
#endif
    AllocaInst *alloc = new AllocaInst(typeOf(type), id.name.c_str(), context.currentBlock());
    context.locals()[id.name] = alloc;
    if (initValue != NULL) {
        NAssignExpr assn(id, *initValue);
        assn.codeGen(context);
    }
    return alloc;
}

Value *NArrayType::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating array type " << type.name <<
#endif
}

Value *NRecordType::codeGen(CodeGenContext &context) {

}

Value *NNameType::codeGen(CodeGenContext &context) {

}

Value *NSimpleVar::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating simple variable " << id.name << endl;
#endif
    // return context.valueDecs[name_]->read(context);
}

Value *NFieldVar::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating field variable " << var.name << " " << id.name << endl;
#endif
    return builder.CreateGEP(
        type, // TODO
        builder.CreateLoad(var->codeGen(context), ""),
        ConstantInt::get(Type::getInt64Ty(MyContext), APInt(64, id)),
        "");
}

Value *NSubscriptVar::codeGen(CodeGenContext &context) {
#ifdef DEBUG
    std::cout << "Creating subscription variable " << var.name << " " << id.name << endl;
#endif
    return builder.CreateGEP(
        type, // TODO
        builder.CreateLoad(var->codeGen(context), ""),
        sub->codeGen(context),
        "");
}


Value* Symbol::codeGen(CodeGenContext& context) {
#ifdef DEBUG
    std::cout << "Creating identifier reference " << name << endl;
#endif
    if (context.locals().find(name) == context.locals().end()) {
        std::cerr << "undeclared variable " << name << endl;
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
//         std::cout << "Generating code for " << typeid(**it).name() << endl;
// #endif
//         last = (**it).codeGen(context);
//     }
// #ifdef DEBUG
//     std::cout << "Creating block" << endl;
// #endif
//     return last;
// }

// Value* NExpressionStatement::codeGen(CodeGenContext& context)
// {
// #ifdef DEBUG
//     std::cout << "Generating code for " << typeid(expression).name() << endl;
// #endif
//     return expression.codeGen(context);
// }

// Value* NReturnStatement::codeGen(CodeGenContext& context)
// {
// #ifdef DEBUG
//     std::cout << "Generating return code for " << typeid(expression).name() << endl;
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


