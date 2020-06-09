#include "node.h"
#include "codegen.h"
#include "myparser.hpp"


/* Compile the AST into a module */
void CodeGenContext::generateCode(NExpr *root) {
// #ifdef DEBUG
//     std::cout << "Generating code...\n";
// #endif
    /* Create the top level interpreter function to call as entry */
    std::vector<llvm::Type *> argTypes;
    llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(MyContext), llvm::makeArrayRef(argTypes), false);
    mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, "main", module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(MyContext, "entry", mainFunction, 0);

    /* Push a new variable/block context */
    pushBlock(bblock);
    root->codeGen(*this); /* emit bytecode for the toplevel block */
    llvm::ReturnInst::Create(MyContext, bblock);
    popBlock();

// #ifdef DEBUG
//     std::cout << "Code is generated.\n";
// #endif
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

    pm.add(llvm::createPrintModulePass(outs()));
    pm.run(*module);
}

/* Executes the AST by running the main function */
llvm::GenericValue CodeGenContext::runCode() {
// #ifdef DEBUG
//     std::cout << "Running code...\n";
// #endif
    llvm::ExecutionEngine *ee = llvm::EngineBuilder(unique_ptr<llvm::Module>(module)).create();
    ee->finalizeObject();
    std::vector<llvm::GenericValue> noargs;
    llvm::GenericValue v = ee->runFunction(mainFunction, noargs);
// #ifdef DEBUG
//     std::cout << "Code was run.\n";
// #endif
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

}

// llvm::Value *NDecl::codeGen(CodeGenContext &context) {}

llvm::Value *NDeclList::codeGen(CodeGenContext &context) {

}

// llvm::Value *NVar::codeGen(CodeGenContext &context) {}

llvm::Value *NVarList::codeGen(CodeGenContext &context) {

}

// llvm::Value *NType::codeGen(CodeGenContext &context) {}

llvm::Value *NFieldTypeList::codeGen(CodeGenContext &context) {

}

llvm::Value *NFieldExprList::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating field expression list " << value << std::endl;
// #endif
}

llvm::Value *NStrExpr::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating string " << value << std::endl;
// #endif
    return builder.CreateGlobalStringPtr(value, "str");
}

llvm::Value *NIntExpr::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating integer " << value << std::endl;
// #endif
    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(MyContext), value, true);
}

llvm::Value *NNilExpr::codeGen(CodeGenContext &context) { // TODO: verify null pointer
// #ifdef DEBUG
//     std::cout << "Creating nil" << std::endl;
// #endif
    return llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(llvm::StructType::get(MyContext)));
}

llvm::Value *NOpExpr::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating binary operation " << op << std::endl;
// #endif
    llvm::Instruction::BinaryOps minst;
    llvm::ICmpInst::Predicate cinst;
    switch (op) {
        case PLUS: minst = llvm::Instruction::Add; goto math;
        case MINUS: minst = llvm::Instruction::Sub; goto math;
        case TIMES: minst = llvm::Instruction::Mul; goto math;
        case DIVIDE: minst = llvm::Instruction::SDiv; goto math;
        case AND: minst = llvm::Instruction::And; goto math;
        case OR: minst = llvm::Instruction::Or; goto math;
        // case XOR: inst = llvm::Instruction::Xor; goto math;
        case EQ: cinst = llvm::ICmpInst::ICMP_EQ; goto cmp;
        case NEQ: cinst = llvm::ICmpInst::ICMP_NE; goto cmp; 
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
// #ifdef DEBUG
//     std::cout << "Creating assignment for" << std::endl;
// #endif
    // if (context.venv.findAll(var.name) == context.locals().end()) {
    //     std::cerr << "Undeclared variable " << var.name << std::endl;
    //     return nullptr;
    // }
    return new llvm::StoreInst(rhs->codeGen(context), var->codeGen(context), false, context.currentBlock());
}

llvm::Value *NRecordExpr::codeGen(CodeGenContext &context) { // TODO
// #ifdef DEBUG
//     std::cout << "Creating record for " << *type << std::endl;
// #endif
    auto etype = llvm::cast<llvm::PointerType>(context.castType(type))->getElementType();
    auto size = context.module->getDataLayout().getTypeAllocSize(etype);
    llvm::Value *var = builder.CreateCall(
        context.createIntrinsicFunction( // TODO
            "",
            {llvm::Type::getInt64Ty(MyContext)},
            llvm::Type::getInt8PtrTy(MyContext))
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(MyContext), llvm::APInt(64, size)),
        "");
    var = builder.CreateBitCast(var, type->codeGen(context), "");
    size_t idx = 0;
    for (NFieldExprList *it = fields; it != nullptr; it = it->next) {
        auto exp = it->codeGen(context);
        auto eptr = builder.CreateGEP(
            context.castType(it->id), // TODO
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
// #ifdef DEBUG
//     std::cout << "Creating array" << std::endl;
// #endif
    llvm::ArrayType* arrayTy = llvm::ArrayType::get(llvm::IntegerType::get(MyContext, 64), size->codeGen(context)); // TODO: int to type
    return new llvm::AllocaInst(arrayTy, "", context.currentBlock());
}

llvm::Value *NCallExpr::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating function call " << func << std::endl;
// #endif
    llvm::Function *function = context.module->getFunction(func->id);
    if (function == nullptr)
        std::cerr << "No such function: " << func << std::endl;

    std::vector<llvm::Value *> args;
    for (NExprList *it = this->args; it != nullptr; it = it->next)
        args.push_back((*it).codeGen(context));

    return llvm::CallInst::Create(function, llvm::makeArrayRef(args), "", context.currentBlock());;
}

llvm::Value *NSeqExpr::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating sequence" << std::endl;
// #endif
    llvm::Value *last = nullptr;
    for (NExprList *it = exprs; it != nullptr; it = it->next)
        last = it->head->codeGen(context);
    return last;
}

llvm::Value *NIfExpr::codeGen(CodeGenContext &context) { // TODO: verify
// #ifdef DEBUG
//     std::cout << "Creating if" << std::endl;
// #endif
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
// #ifdef DEBUG
//     std::cout << "Creating while loop" << std::endl;
// #endif
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
// #ifdef DEBUG
//     std::cout << "Creating for loop" << std::endl;
// #endif
    llvm::Function *function = builder.GetInsertBlock()->getParent();

    llvm::BasicBlock *testBlock = llvm::BasicBlock::Create(MyContext, "", function);
    llvm::BasicBlock *loopBlock = llvm::BasicBlock::Create(MyContext, "", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(MyContext, "", function);
    llvm::BasicBlock *afterBlock = llvm::BasicBlock::Create(MyContext, "", function);
    context.loopstk.push({nextBlock, afterBlock});

    builder.CreateBr(testBlock);
    builder.SetInsertPoint(testBlock);

    llvm::Value *vardec = id->codeGen(context);
    auto cond = builder.CreateICmpSLE(
        builder.CreateLoad(vardec, ""), high->codeGen(context), "");

    builder.CreateCondBr(cond, loopBlock, afterBlock);
    builder.SetInsertPoint(loopBlock);

    context.venv.enterScope();
    context.venv.push(id->id, vardec);

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
// #ifdef DEBUG
//     std::cout << "Creating break" << std::endl;
// #endif
    builder.CreateBr(std::get<1>(context.loopstk.top()));
    return llvm::Constant::getNullValue(llvm::Type::getInt64Ty(MyContext)); // nothing
}

llvm::Value *NLetExpr::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating let" << std::endl;
// #endif
    context.tenv.enterScope();
    context.venv.enterScope();
    for (auto &dec = decls; dec != nullptr; dec = dec->next)
        dec->codeGen(context);
    auto result = body->codeGen(context);
    context.venv.quitScope();
    context.tenv.quitScope();
    return result;
}

llvm::Value *NFuncDecl::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating function " << id.name << std::endl;
// #endif
    std::vector<llvm::Type *> argTypes;
    for (NFieldTypeList *it = params; it != nullptr; it = it->next)
        argTypes.push_back(context.castType(it->type));

    llvm::FunctionType *ftype = llvm::FunctionType::get(context.castType(retType), llvm::makeArrayRef(context.castType(params->type)), false);
    llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, "", context.module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(MyContext, "entry", function, 0);

    context.pushBlock(bblock);
    context.tenv.enterScope();
    context.venv.enterScope();

    llvm::Function::arg_iterator argsValues = function->arg_begin();
    llvm::Value* argumentValue;

    for (NFieldTypeList *it = params; it != nullptr; it = it->next) {
        (*it).codeGen(context);

        argumentValue = &*argsValues++;
        // argumentValue->setName((*it)->id.name.c_str());
        llvm::StoreInst *inst = new llvm::StoreInst(argumentValue, context.venv.findAll(it->id), false, bblock);
    }
    
    body->codeGen(context);
    llvm::ReturnInst::Create(MyContext, context.getCurrentReturnValue(), bblock);

    context.popBlock();
    context.venv.quitScope();
    context.tenv.quitScope();

    return function;
}

llvm::Value *NTypeDecl::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating type declaration " << type.name << " " << id.name << std::endl;
// #endif

}

llvm::Value *NVarDecl::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating variable declaration " << type.name << " " << id.name << std::endl;
// #endif
    llvm::AllocaInst *alloc = new llvm::AllocaInst(context.castType(type), "", context.currentBlock()); // TODO
    context.venv.push(*id, alloc); // TODO: modify env
    if (initValue != nullptr) {
        NAssignExpr assn(line, index, id, initValue);
        assn.codeGen(context);
    }
    return alloc;
}

llvm::Value *NArrayType::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating array type " << type.name <<
// #endif
}

llvm::Value *NRecordType::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating record type" << std::endl;
// #endif
}

llvm::Value *NNameType::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating name type " << *id << std::endl;
// #endif
}

llvm::Value *NSimpleVar::codeGen(CodeGenContext &context) {
// #ifdef DEBUG
//     std::cout << "Creating simple variable " << *id << std::endl;
// #endif
    // return context.valueDecs[name_];
    return context.venv.findAll(*id);
}

// llvm::Value *NFieldVar::codeGen(CodeGenContext &context) {
// // #ifdef DEBUG
// //     std::cout << "Creating field variable " << var.id << " " << id << std::endl;
// // #endif
//     return builder.CreateGEP(
//         context.castType(var), // TODO
//         builder.CreateLoad(var->codeGen(context), ""),
//         llvm::ConstantInt::get(llvm::Type::getInt64Ty(MyContext), llvm::APInt(64, id)),
//         "");
// }

// llvm::Value *NSubscriptVar::codeGen(CodeGenContext &context) {
// // #ifdef DEBUG
// //     std::cout << "Creating subscription variable" << std::endl;
// // #endif
//     return builder.CreateGEP(
//         context.castType(var), // TODO
//         builder.CreateLoad(var->codeGen(context), ""),
//         sub->codeGen(context),
//         "");
// }

llvm::Value *Symbol::codeGen(CodeGenContext& context) {
// #ifdef DEBUG
//     std::cout << "Creating identifier reference " << id << std::endl;
// #endif
    // if (context.locals().find(id) == context.locals().end()) {
    //     std::cerr << "Undeclared variable " << id << std::endl;
    //     return nullptr;
    // }
    return new llvm::LoadInst(context.venv.findAll(*this), "", false, context.currentBlock());
}
