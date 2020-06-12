#include "codegen.h"
#include "myparser.hpp"

/* Built-in symbol bindings */
void CodeGenContext::initEnv() {
    /* external functions */
    venv.push(Symbol("print"), createIntrinsicFunction("print", {lpint8}, lvoid));
    venv.push(Symbol("printi"), createIntrinsicFunction("printi", {lint64}, lvoid));
    // venv.push(Symbol("flush"), createIntrinsicFunction("flush", {lint64}, lvoid));
    // venv.push(Symbol("getchar"), createIntrinsicFunction("getchar", {lint64}, lvoid));
    // venv.push(Symbol("ord"), createIntrinsicFunction("ord", {lint64}, lvoid));
    // venv.push(Symbol("chr"), createIntrinsicFunction("chr", {lint64}, lvoid));
    // venv.push(Symbol("size"), createIntrinsicFunction("size", {lint64}, lvoid));
    // venv.push(Symbol("substring"), createIntrinsicFunction("substring", {lint64}, lvoid));
    // venv.push(Symbol("concat"), createIntrinsicFunction("concat", {lint64}, lvoid));
    // venv.push(Symbol("not"), createIntrinsicFunction("not", {lint64}, lvoid));
    // venv.push(Symbol("exit"), createIntrinsicFunction("exit", {lint64}, lvoid));
    /* predefined types */
    tenv.push(Symbol("int"), lint64);
    tenv.push(Symbol("string"), lpint8);
}

/* Compile the AST into a module */
void CodeGenContext::generateCode(NExpr *root) {
#ifdef _DEBUG
    std::cout << "Generating code..." << std::endl;
#endif
    initEnv();

    /* Create the top level interpreter function to call as entry */
    std::vector<llvm::Type *> argTypes;
    llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(MyContext), llvm::makeArrayRef(argTypes), false);
    mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, "main", module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(MyContext, "entry", mainFunction, 0);

    builder.SetInsertPoint(bblock);

    /* Push a new variable/block context */
    pushBlock(bblock);
    root->codeGen(*this); /* emit bytecode for the toplevel block */
    // llvm::ReturnInst::Create(MyContext, currentBlock());
    builder.CreateRet(lzero);
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

llvm::Function *CodeGenContext::createIntrinsicFunction(
        const std::string &name,
        const std::vector<llvm::Type *> &args,
        llvm::Type *retType) {
    auto ftype = llvm::FunctionType::get(retType, args, false);
    auto function = llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, name, module);
    venv.push(Symbol(name), function); // TODO
    return function;
}

/* Code Generation */

llvm::Value *NExprList::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating expression list" << std::endl;
#endif
    llvm::Value *last = head->codeGen(context);
    if (next != nullptr)
        last = next->codeGen(context);
    return last;
}

llvm::Value *NDeclList::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating declaration list" << std::endl;
#endif
    llvm::Value *last = head->codeGen(context);
    if (next != nullptr)
        last = next->codeGen(context);
    return last;
}

llvm::Value *NVarList::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating variable list" << std::endl;
#endif
    llvm::Value *last = head->codeGen(context);
    if (next != nullptr)
        last = next->codeGen(context);
    return last;
}

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
    std::cout << "Creating string: " << value << std::endl;
#endif
    return builder.CreateGlobalStringPtr(value, "str");
}

llvm::Value *NIntExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating integer: " << value << std::endl;
#endif
    return llvm::ConstantInt::get(lint64, value, true);
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
    return builder.CreateLoad(var->codeGen(context));
}

llvm::Value *NOpExpr::codeGen(CodeGenContext &context) { // TODO: builder api
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
    return builder.CreateStore(rhs->codeGen(context), var->codeGen(context));
}

llvm::Value *NRecordExpr::codeGen(CodeGenContext &context) { // TODO: test
#ifdef _DEBUG
    std::cout << "Creating record" << std::endl;
#endif
    auto rtype = context.tenv.findAll(*type);
    auto etype = llvm::cast<llvm::PointerType>(rtype)->getElementType();
    auto esize = context.module->getDataLayout().getTypeAllocSize(etype);
    auto var = builder.CreateBitCast(
        builder.CreateCall(
            context.createIntrinsicFunction("", {lint64, lint64}, lpint8),
            llvm::ConstantInt::get(lint64, llvm::APInt(64, esize))),
        rtype);

    size_t idx = 0;
    for (NFieldExprList *it = fields; it != nullptr; it = it->next) {
        auto exp = it->codeGen(context);
        auto eptr = builder.CreateGEP(
            context.tenv.findAll(*(it->id)),
            var,
            llvm::ConstantInt::get(lint64, llvm::APInt(64, idx)));
        builder.CreateStore(exp, eptr);
        ++idx;
    }
    return var;
}

llvm::Value *NArrayExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating array expression" << std::endl;
#endif
    auto init = initValue->codeGen(context);
    auto asize = size->codeGen(context);
    auto etype = context.tenv.findAll(*type);
    assert(init->getType() == etype);
    auto esize = llvm::ConstantInt::get(lint64, llvm::APInt(64, context.module->getDataLayout().getTypeAllocSize(etype)));
    auto aptr = builder.CreateBitCast(
        builder.CreateCall(
            context.createIntrinsicFunction("allocaArray", {lint64, lint64}, lpint8),
            std::vector<llvm::Value *>{asize, esize}),
        etype);
    // auto aptr = builder.CreateAlloca(llvm::ArrayType::get(etype, 10));

    /* initialize with a loop */
    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *testBlock = llvm::BasicBlock::Create(MyContext, "test", function);
    llvm::BasicBlock *loopBlock = llvm::BasicBlock::Create(MyContext, "loop", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(MyContext, "next", function);
    llvm::BasicBlock *afterBlock = llvm::BasicBlock::Create(MyContext, "after", function);

    // llvm::IRBuilder<> tbuilder(&function->getEntryBlock(), function->getEntryBlock().begin());
    auto iptr = builder.CreateAlloca(lint64, nullptr);
    builder.CreateStore(lzero, iptr);

    builder.CreateBr(testBlock);

    builder.SetInsertPoint(testBlock);
    auto index = builder.CreateLoad(iptr);
    auto cond = builder.CreateICmpSLE(index, asize);
    builder.CreateCondBr(cond, loopBlock, afterBlock);

    builder.SetInsertPoint(loopBlock);
    builder.CreateStore(init, builder.CreateGEP(etype, aptr, index)); // init
    // builder.CreateBr(nextBlock);

    builder.SetInsertPoint(nextBlock);
    builder.CreateStore(builder.CreateAdd(index, lone), iptr);
    builder.CreateBr(testBlock);

    builder.SetInsertPoint(afterBlock);

    return aptr;
}

llvm::Value *NCallExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating function call: " << func->id << std::endl;
#endif
    // llvm::Function *function = context.module->getFunction(func->id);
    auto function = llvm::cast<llvm::Function>(context.venv.findAll(*func));

    std::vector<llvm::Value *> fargs;
    for (NExprList *it = args; it != nullptr; it = it->next)
        fargs.push_back((*it).codeGen(context));

    return builder.CreateCall(function, llvm::makeArrayRef(fargs));
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
    auto function = builder.GetInsertBlock()->getParent();

    auto thenBlock = llvm::BasicBlock::Create(MyContext, "then", function);
    auto elseBlock = llvm::BasicBlock::Create(MyContext, "else");
    auto mergeBlock = llvm::BasicBlock::Create(MyContext, "merge");

    auto cond = builder.CreateICmpNE(test->codeGen(context), lzero);
    builder.CreateCondBr(cond, thenBlock, elseBlock);

    builder.SetInsertPoint(thenBlock);
    context.pushBlock(thenBlock);
    auto thenValue = thenClause->codeGen(context);
    context.popBlock();
    builder.CreateBr(mergeBlock);
    function->getBasicBlockList().push_back(elseBlock);

    builder.SetInsertPoint(elseBlock);
    llvm::Value *elseValue;
    if (elseClause) {
        context.pushBlock(elseBlock);
        elseValue = elseClause->codeGen(context);
        context.popBlock();
    }
    builder.CreateBr(mergeBlock);
    function->getBasicBlockList().push_back(mergeBlock);

    builder.SetInsertPoint(mergeBlock);
    if (elseClause && !thenValue->getType()->isVoidTy() && !elseValue->getType()->isVoidTy()) {
        auto pn = builder.CreatePHI(thenValue->getType(), 2);
        // thenValue = context.convertNil(thenValue, elseValue);
        // elseValue = context.convertNil(elseValue, thenValue);
        pn->addIncoming(thenValue, thenBlock);
        pn->addIncoming(elseValue, elseBlock);
        return pn;
    }

    return lnull;
}

llvm::Value *NWhileExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating while loop" << std::endl;
#endif
    auto function = builder.GetInsertBlock()->getParent();
    auto testBlock = llvm::BasicBlock::Create(MyContext, "test", function);
    auto loopBlock = llvm::BasicBlock::Create(MyContext, "loop", function);
    auto nextBlock = llvm::BasicBlock::Create(MyContext, "next", function);
    auto afterBlock = llvm::BasicBlock::Create(MyContext, "after", function);
    context.loopstk.push({nextBlock, afterBlock});

    builder.CreateBr(testBlock);

    builder.SetInsertPoint(testBlock);
    context.pushBlock(testBlock);
    auto cond = builder.CreateICmpEQ(test->codeGen(context), lzero);
    context.popBlock();
    builder.CreateCondBr(cond, afterBlock, loopBlock);

    builder.SetInsertPoint(loopBlock);
    context.pushBlock(loopBlock);
    body->codeGen(context);
    context.popBlock();
    builder.CreateBr(nextBlock);

    builder.SetInsertPoint(nextBlock);
    builder.CreateBr(testBlock);

    builder.SetInsertPoint(afterBlock);

    context.loopstk.pop();

    return lnull;
}

llvm::Value *NForExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating for expression" << std::endl;
#endif
    llvm::Function *function = builder.GetInsertBlock()->getParent();

    llvm::BasicBlock *testBlock = llvm::BasicBlock::Create(MyContext, "test", function);
    llvm::BasicBlock *loopBlock = llvm::BasicBlock::Create(MyContext, "loop", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(MyContext, "next", function);
    llvm::BasicBlock *afterBlock = llvm::BasicBlock::Create(MyContext, "after", function);
    context.loopstk.push({nextBlock, afterBlock});

    llvm::Value *index = id->codeGen(context);
    context.venv.enterScope();
    context.venv.push(*(id->id), index);

    builder.CreateBr(testBlock);

    builder.SetInsertPoint(testBlock);
    context.pushBlock(testBlock);
    auto cond = builder.CreateICmpSLE(builder.CreateLoad(index), high->codeGen(context));
    context.popBlock();
    builder.CreateCondBr(cond, loopBlock, afterBlock);

    builder.SetInsertPoint(loopBlock);
    context.pushBlock(loopBlock);
    body->codeGen(context);
    context.popBlock();
    builder.CreateBr(nextBlock);

    builder.SetInsertPoint(nextBlock);
    auto nindex = builder.CreateAdd(builder.CreateLoad(index), lone);
    builder.CreateStore(nindex, index);
    builder.CreateBr(testBlock);

    builder.SetInsertPoint(afterBlock);

    context.venv.quitScope();
    context.loopstk.pop();

    return lnull;
}

llvm::Value *NBreakExpr::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating break" << std::endl;
#endif
    builder.CreateBr(std::get<1>(context.loopstk.top())); // to afterBlock
    return lnull;
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

    llvm::Type *rtype;
    if (!retType)
        rtype = lvoid;
    else
        rtype = retType->typeGen(context);

    llvm::FunctionType *ftype = llvm::FunctionType::get(rtype, llvm::makeArrayRef(argTypes), false);
    llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, "", context.module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(MyContext, "entry", function, 0);

    context.pushBlock(bblock);
    context.tenv.enterScope();
    context.venv.enterScope();

    llvm::Function::arg_iterator argsValues = function->arg_begin();
    llvm::Value* argumentValue;

    for (NFieldTypeList *it = params; it != nullptr; it = it->next) {
        it->codeGen(context);
        argumentValue = &*argsValues++;
        argumentValue->setName(it->id->id);
        llvm::StoreInst *inst = new llvm::StoreInst(argumentValue, context.venv.findAll(*(it->id)), false, bblock);
    }
    
    body->codeGen(context);
    llvm::ReturnInst::Create(MyContext, context.getCurrentReturnValue(), bblock);

    context.venv.quitScope();
    context.tenv.quitScope();
    context.popBlock();

    return function;
}

llvm::Value *NTypeDecl::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating type declaration: " << id->id << std::endl;
#endif
    context.tenv.push(*id, type->typeGen(context));
    if (next)
        next->codeGen(context);

    return lnull;
}

llvm::Value *NVarDecl::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating variable declaration: " << id->id << std::endl;
#endif
    auto init = initValue->codeGen(context);
    llvm::Type *vtype;
    if (!type)
        vtype = init->getType(); // type derivation
    else
        vtype = type->typeGen(context);
    auto alloc = builder.CreateAlloca(vtype, nullptr, id->id.c_str());
    if (initValue != nullptr)
        builder.CreateStore(init, alloc);
    context.venv.push(*id, alloc);

    return alloc;
}

llvm::Value *NArrayType::codeGen(CodeGenContext &context) {}

llvm::Type *NArrayType::typeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Fetching array type: " << id->id << std::endl;
#endif
    return context.tenv.findAll(*id);
}

llvm::Value *NRecordType::codeGen(CodeGenContext &context) {}

llvm::Type *NRecordType::typeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Fetching record type" << std::endl;
#endif
    // return context
}

llvm::Value *NNameType::codeGen(CodeGenContext &context) {}

llvm::Type *NNameType::typeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Fetching name type: " << id->id << std::endl;
#endif
    return context.tenv.findAll(*id);
}

llvm::Value *NSimpleVar::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating simple variable: " << id->id << std::endl;
#endif
    return context.venv.findAll(*id);
}

llvm::Value *NFieldVar::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating field variable: " << id->id << std::endl;
#endif
    // auto avar = var->codeGen(context);
    // return builder.CreateGEP(
    //     avar->getType(), // TODO
    //     builder.CreateLoad(avar),
    //     llvm::ConstantInt::get(lint64, llvm::APInt(64, id))); // TODO: id
}

llvm::Value *NSubscriptVar::codeGen(CodeGenContext &context) {
#ifdef _DEBUG
    std::cout << "Creating subscription variable" << std::endl;
#endif
    auto avar = var->codeGen(context);
    return builder.CreateGEP(
        avar->getType(),
        builder.CreateLoad(avar),
        sub->codeGen(context));
}
