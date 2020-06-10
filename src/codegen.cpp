#include "node.h"
#include "codegen.h"
#include "myparser.hpp"


/* Compile the AST into a module */
void CodeGenContext::generateCode(NExpr *root) {
#ifdef _DEBUG
    std::cout << "Generating code..." << std::endl;
#endif
    /* Create the top level interpreter function to call as entry */
    std::vector<llvm::Type *> argTypes;
    llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(MyContext), llvm::makeArrayRef(argTypes), false);
    mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, "main", module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(MyContext, "entry", mainFunction, 0);

    builder.SetInsertPoint(bblock);

    /* Push a new variable/block context */
    pushBlock(bblock);
    root->codeGen(*this); /* emit bytecode for the toplevel block */
    llvm::ReturnInst::Create(MyContext, bblock);
    popBlock();

#ifdef _DEBUG
    std::cout << "Code is generated." << std::endl;
#endif
    /* Print the bytecode in a human-readable format */
    // module->dump();

    llvm::legacy::PassManager pm;

    /* Optimizations */
    // pm.add(createBasicAliasAnalysisPass());
    // pm.add(createPromoteMemoryToRegisterPass());
    // pm.add(createInstructionCombiningPass());
    // pm.add(createReassociatePass());
    // pm.add(createGVNPass());
    // pm.add(createCFGSimplificationPass());

    pm.add(llvm::createPrintModulePass(llvm::outs()));
    pm.run(*module);
}

/* Executes the AST by running the main function */
llvm::GenericValue CodeGenContext::runCode() {
#ifdef _DEBUG
    std::cout << "Running code..." << std::endl;
#endif
    llvm::ExecutionEngine *ee = llvm::EngineBuilder(unique_ptr<llvm::Module>(module)).create();
    ee->finalizeObject();
    std::vector<llvm::GenericValue> noargs;
    llvm::GenericValue v = ee->runFunction(mainFunction, noargs);
#ifdef _DEBUG
    std::cout << "Code was run." << std::endl;
#endif
    return v;
}

/* Type to llvm::Type */
llvm::Type *CodeGenContext::castType(Type *type) {
    llvm::Type *ltype = nullptr;
    switch(type->type) {
        case Type::TInt: ltype = llvm::Type::getInt64Ty(MyContext); break;
        case Type::TString: ltype = nullptr; break;
        case Type::TNil: ltype = nullptr; break;
    }
    return ltype;
}

llvm::Function *CodeGenContext::createIntrinsicFunction(
        const std::string &name,
        const std::vector<llvm::Type *> &args,
        llvm::Type *retType) {
    auto functionType = llvm::FunctionType::get(retType, args, false);
    auto function = llvm::Function::Create(
        functionType, llvm::Function::ExternalLinkage, name, module);
    venv.push(Symbol(name), function); // TODO
    return function;
}

/* Code Generation */

// llvm::Value *NExpr::codeGen(CodeGenContext &context) {}

llvm::Value *NExprList::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating expression list" << std::endl;
#endif
    llvm::Value *last = head->codeGen(context);
    if (next != nullptr)
        last = next->codeGen(context);
    return last;
}

// llvm::Value *NDecl::codeGen(CodeGenContext &context) {}

llvm::Value *NDeclList::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating declaration list" << std::endl;
#endif
}

// llvm::Value *NVar::codeGen(CodeGenContext &context) {}

llvm::Value *NVarList::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating variable list" << std::endl;
#endif
}

// llvm::Value *NType::codeGen(CodeGenContext &context) {}

llvm::Value *NFieldTypeList::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating field type list" << std::endl;
#endif
}

llvm::Value *NFieldExprList::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating field expression list" << std::endl;
#endif
}

llvm::Value *NStrExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating string" << std::endl;
#endif
    return builder.CreateGlobalStringPtr(value, "str");
}

llvm::Value *NIntExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating integer: " << value << std::endl;
#endif
    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(MyContext), value, true);
}

llvm::Value *NNilExpr::codeGen(CodeGenContext &context) { // TODO: verify null pointer
#ifdef _DEBUG
    std::cout << "Creating nil" << std::endl;
#endif
    return llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(llvm::StructType::get(MyContext)));
}

llvm::Value *NVarExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating variable expression" << std::endl;
#endif
    return builder.CreateLoad(var->codeGen(context), "");
}

llvm::Value *NOpExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating binary operation: " << op << std::endl;
#endif
    llvm::Instruction::BinaryOps minst;
    llvm::ICmpInst::Predicate cinst;
    switch (op) {
        case PLUS: minst = llvm::Instruction::Add; goto math;
        case MINUS: minst = llvm::Instruction::Sub; goto math;
        case MUL: minst = llvm::Instruction::Mul; goto math;
        case DIV: minst = llvm::Instruction::SDiv; goto math;
        case AND: minst = llvm::Instruction::And; goto math;
        case OR: minst = llvm::Instruction::Or; goto math;
        // case XOR: inst = llvm::Instruction::Xor; goto math;
        case EQ: cinst = llvm::ICmpInst::ICMP_EQ; goto cmp;
        case NE: cinst = llvm::ICmpInst::ICMP_NE; goto cmp;
        case LT: cinst = llvm::ICmpInst::ICMP_SLT; goto cmp;
        case LE: cinst = llvm::ICmpInst::ICMP_SLE; goto cmp;
        case GT: cinst = llvm::ICmpInst::ICMP_SGT; goto cmp;
        case GE: cinst = llvm::ICmpInst::ICMP_SGE; goto cmp;
    }
    return nullptr;
math:
    return llvm::BinaryOperator::Create(minst, lhs->codeGen(context), 
        rhs->codeGen(context), "", context.currentBlock());
cmp:
    return new llvm::ZExtInst( // compare and cast
        new llvm::ICmpInst(*context.currentBlock(), cinst, lhs->codeGen(context), rhs->codeGen(context), ""),
        llvm::IntegerType::get(MyContext, 64), "", context.currentBlock());
}

llvm::Value *NAssignExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating assignment" << std::endl;
#endif
    // if (context.venv.findAll(var.name) == context.locals().end()) {
    //     std::cerr << "Undeclared variable " << var.name << std::endl;
    //     return nullptr;
    // }
    return new llvm::StoreInst(rhs->codeGen(context), var->codeGen(context), false, context.currentBlock());
}

llvm::Value *NRecordExpr::codeGen(CodeGenContext &context) { // TODO
#ifdef _DEBUG
    std::cout << "Creating record" << std::endl;
#endif
    auto etype = llvm::cast<llvm::PointerType>(context.tenv.findAll(*type))->getElementType();
    auto size = context.module->getDataLayout().getTypeAllocSize(etype);
    llvm::Value *var = builder.CreateCall(
        context.createIntrinsicFunction( // TODO
            "",
            {llvm::Type::getInt64Ty(MyContext)},
            llvm::Type::getInt8PtrTy(MyContext)),
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(MyContext), llvm::APInt(64, size)),
        "");
    var = builder.CreateBitCast(var, context.tenv.findAll(*type), "");
    size_t idx = 0;
    for (NFieldExprList *it = fields; it != nullptr; it = it->next) {
        auto exp = it->codeGen(context);
        auto eptr = builder.CreateGEP(
            context.tenv.findAll(*(it->id)), // TODO
            var,
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(MyContext), llvm::APInt(64, idx)),
            "");
        // context.checkStore(exp, eptr);
        builder.CreateStore(exp, eptr);
        ++idx;
    }
    return var;
}

llvm::Value *NArrayExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating array" << std::endl;
#endif
//     llvm::ArrayType* arrayTy = llvm::ArrayType::get(llvm::IntegerType::get(MyContext, 64), size->codeGen(context)); // TODO: int to type
//     return new llvm::AllocaInst(arrayTy, "", context.currentBlock());
}

llvm::Value *NCallExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating function call" << std::endl;
#endif
    llvm::Function *function = context.module->getFunction(func->id);
    if (function == nullptr)
        std::cerr << "No such function" << std::endl;

    std::vector<llvm::Value *> args;
    for (NExprList *it = this->args; it != nullptr; it = it->next)
        args.push_back((*it).codeGen(context));

    return llvm::CallInst::Create(function, llvm::makeArrayRef(args), "", context.currentBlock());;
}

llvm::Value *NSeqExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating sequence" << std::endl;
#endif
    llvm::Value *last = nullptr;
    for (NExprList *it = exprs; it != nullptr; it = it->next)
        last = it->head->codeGen(context);
    return last;
}

llvm::Value *NIfExpr::codeGen(CodeGenContext &context) { // TODO: verify
#ifdef _DEBUG
    std::cout << "Creating if" << std::endl;
#endif
    auto cond = builder.CreateICmpNE(test->codeGen(context), llvm::ConstantInt::get(llvm::Type::getInt64Ty(MyContext), llvm::APInt(64, 0)), "");
    auto function = builder.GetInsertBlock()->getParent();

    auto thenBlock = llvm::BasicBlock::Create(MyContext, "", function);
    auto elseBlock = llvm::BasicBlock::Create(MyContext, "");
    auto mergeBlock = llvm::BasicBlock::Create(MyContext, "");

    builder.CreateCondBr(cond, thenBlock, elseBlock);
    builder.SetInsertPoint(thenBlock);

    auto then = thenClause->codeGen(context);
    builder.CreateBr(mergeBlock);

    thenBlock = builder.GetInsertBlock();
    function->getBasicBlockList().push_back(elseBlock);
    builder.SetInsertPoint(elseBlock);

    llvm::Value *elseValue;
    if (elseClause) {
        elseValue = elseClause->codeGen(context);
        if (!elseValue)
            return nullptr;
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

    return llvm::Constant::getNullValue(llvm::Type::getInt64Ty(MyContext));
}

llvm::Value *NWhileExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating while loop" << std::endl;
#endif
    auto function = builder.GetInsertBlock()->getParent();
    auto testBlock = llvm::BasicBlock::Create(MyContext, "", function);
    auto loopBlock = llvm::BasicBlock::Create(MyContext, "", function);
    auto nextBlock = llvm::BasicBlock::Create(MyContext, "", function);
    auto afterBlock = llvm::BasicBlock::Create(MyContext, "", function);
    context.loopstk.push({nextBlock, afterBlock});

    builder.CreateBr(testBlock);
    builder.SetInsertPoint(testBlock);

    builder.CreateCondBr( // to after or loop
        builder.CreateICmpEQ(
            test->codeGen(context),
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(MyContext), llvm::APInt(64, 0)),
            ""),
        afterBlock,
        loopBlock);
    builder.SetInsertPoint(loopBlock);

    body->codeGen(context);

    builder.CreateBr(nextBlock);
    builder.SetInsertPoint(nextBlock);
    builder.CreateBr(testBlock);

    builder.SetInsertPoint(afterBlock);

    context.loopstk.pop();

    return llvm::Constant::getNullValue(llvm::Type::getInt64Ty(MyContext));
}

llvm::Value *NForExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating for expression" << std::endl;
#endif
    llvm::Function *function = builder.GetInsertBlock()->getParent();

    llvm::BasicBlock *testBlock = llvm::BasicBlock::Create(MyContext, "", function);
    llvm::BasicBlock *loopBlock = llvm::BasicBlock::Create(MyContext, "", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(MyContext, "", function);
    llvm::BasicBlock *afterBlock = llvm::BasicBlock::Create(MyContext, "", function);
    context.loopstk.push({nextBlock, afterBlock});

    builder.CreateBr(testBlock);
    builder.SetInsertPoint(testBlock);

    llvm::Value *vardec = id->codeGen(context);
    auto xx = builder.CreateLoad(vardec, "");
    std::cout << "here" << std::endl;
    auto cond = builder.CreateICmpSLE(
        xx, high->codeGen(context), "");
    std::cout << "here" << std::endl;

    builder.CreateCondBr(cond, loopBlock, afterBlock);
    builder.SetInsertPoint(loopBlock);
    std::cout << "here" << std::endl;

    context.venv.enterScope();
    context.venv.push(*(id->id), vardec);

    if (!body->codeGen(context))
        return nullptr;

    builder.CreateBr(nextBlock);
    builder.SetInsertPoint(nextBlock);

    auto nextVar = builder.CreateAdd(
        builder.CreateLoad(vardec, ""),
        llvm::ConstantInt::get(MyContext, llvm::APInt(64, 1)), "");
    builder.CreateStore(nextVar, vardec);

    builder.CreateBr(testBlock);
    builder.SetInsertPoint(afterBlock);

    context.venv.quitScope();
    context.loopstk.pop();

    return llvm::Constant::getNullValue(llvm::Type::getInt64Ty(MyContext));
}

llvm::Value *NBreakExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating break" << std::endl;
#endif
    builder.CreateBr(std::get<1>(context.loopstk.top()));
    return llvm::Constant::getNullValue(llvm::Type::getInt64Ty(MyContext)); // nothing
}

llvm::Value *NLetExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating let" << std::endl;
#endif
    context.tenv.enterScope();
    context.venv.enterScope();
    for (auto &dec = decls; dec != nullptr; dec = dec->next)
        dec->head->codeGen(context);
    auto result = body->codeGen(context);
    context.venv.quitScope();
    context.tenv.quitScope();
    return result;
}

llvm::Value *NFuncDecl::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating function: " << id->id << std::endl;
#endif
    std::vector<llvm::Type *> argTypes;
    for (NFieldTypeList *it = params; it != nullptr; it = it->next)
        argTypes.push_back(context.tenv.findAll(*(it->type)));

    llvm::FunctionType *ftype = llvm::FunctionType::get((llvm::Type *)(retType->codeGen(context)), llvm::makeArrayRef(context.tenv.findAll(*(params->type))), false);
    std::cout << "here" << std::endl;
    llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, "", context.module);
    std::cout << "here" << std::endl;
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(MyContext, "entry", function, 0);
    std::cout << "here" << std::endl;

    context.pushBlock(bblock);
    context.tenv.enterScope();
    context.venv.enterScope();

    llvm::Function::arg_iterator argsValues = function->arg_begin();
    llvm::Value* argumentValue;
    std::cout << "here" << std::endl;

    for (NFieldTypeList *it = params; it != nullptr; it = it->next) {
        (*it).codeGen(context);

        argumentValue = &*argsValues++;
        // argumentValue->setName((*it)->id.name.c_str());
        llvm::StoreInst *inst = new llvm::StoreInst(argumentValue, context.venv.findAll(*(it->id)), false, bblock);
    }
    std::cout << "here" << std::endl;
    
    body->codeGen(context);
    llvm::ReturnInst::Create(MyContext, context.getCurrentReturnValue(), bblock);

    context.popBlock();
    context.venv.quitScope();
    context.tenv.quitScope();

    return function;
}

llvm::Value *NTypeDecl::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating type declaration: " << id->id << std::endl;
#endif

}

llvm::Value *NVarDecl::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating variable declaration: " << id->id << std::endl;
#endif
    llvm::Type *vtype = llvm::Type::getVoidTy(MyContext);
    if (type)
        vtype = (llvm::Type *)(type->codeGen(context));
    llvm::AllocaInst *alloc = new llvm::AllocaInst(llvm::Type::getInt64Ty(MyContext), "", context.currentBlock());
    // llvm::AllocaInst *alloc = new llvm::AllocaInst(vtype, "", context.currentBlock());
    std::cout << "here" << std::endl;
    // if (initValue != nullptr) {
    //     NAssignExpr assn(line, index, id, initValue);
    //     assn.codeGen(context);
    // }
    context.venv.push(*id, alloc);
    return alloc;
}

llvm::Value *NArrayType::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating array type " << std::endl;
#endif
}

llvm::Value *NRecordType::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating record type" << std::endl;
#endif
}

llvm::Value *NNameType::codeGen(CodeGenContext &context) { // TODO: value to type
#ifdef _DEBUG
    std::cout << "Creating name type: " << id->id << std::endl;
#endif
    return (llvm::Value *)(context.tenv.findAll(*id));
}

llvm::Value *NSimpleVar::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating simple variable " << std::endl;
#endif
    // return context.valueDecs[name_];
    return context.venv.findAll(*id);
}

llvm::Value *NFieldVar::codeGen(CodeGenContext &context) {
// #ifdef _DEBUG
//     std::cout << "Creating field variable " << var.id << " " << id << std::endl;
// #endif
//     return builder.CreateGEP(
//         context.castType(var), // TODO
//         builder.CreateLoad(var->codeGen(context), ""),
//         llvm::ConstantInt::get(llvm::Type::getInt64Ty(MyContext), llvm::APInt(64, id)),
//         "");
}

llvm::Value *NSubscriptVar::codeGen(CodeGenContext &context) {
// #ifdef _DEBUG
//     std::cout << "Creating subscription variable" << std::endl;
// #endif
//     return builder.CreateGEP(
//         context.castType(var), // TODO
//         builder.CreateLoad(var->codeGen(context), ""),
//         sub->codeGen(context),
//         "");
}

// llvm::Value *Symbol::codeGen(CodeGenContext& context) {
// #ifdef _DEBUG
//     std::cout << "Creating identifier reference " << id << std::endl;
// #endif
//     // if (context.locals().find(id) == context.locals().end()) {
//     //     std::cerr << "Undeclared variable " << id << std::endl;
//     //     return nullptr;
//     // }
//     return new llvm::LoadInst(context.venv.findAll(*this), "", false, context.currentBlock());
// }
