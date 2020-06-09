/* Code generation context is maintained with a stack of blocks. */
#pragma once

#include <stack>
#include <tuple>
#include <typeinfo>

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>

#include <llvm/Bitcode/BitstreamReader.h>
#include <llvm/Bitcode/BitstreamWriter.h>

#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include "env.h"

using namespace llvm;

static LLVMContext MyContext;
static IRBuilder<> builder{MyContext};

class CodeGenBlock {
public:
    BasicBlock *block;
    Value *returnValue;
};

class CodeGenContext {
    std::stack<CodeGenBlock *> blocks;
    Function *mainFunction;

public:
    Module *module;
    SymbolTable<llvm::Value> venv;
    SymbolTable<llvm::Type> tenv;
    std::stack<std::tuple<BasicBlock *, BasicBlock *>> loopstk;

    CodeGenContext() {
        module = new Module("main", MyContext);
        // venv = initLVarEnv();
        // tenv = initLTypeEnv();
    }

    void generateCode(NExpr &root);
    GenericValue runCode();

    llvm::Type *typeOf(Type *type);
    Function *createIntrinsicFunction(
        const std::string &name,
        const std::vector<llvm::Type *> &args,
        llvm::Type *retType);

    // std::map<std::string, Value*> &locals() {
    //     return blocks.top()->locals;
    // }

    /* Block stack operations */
    BasicBlock *currentBlock() {
        return blocks.top()->block;
    }
    void pushBlock(BasicBlock *block) {
        blocks.push(new CodeGenBlock());
        blocks.top()->returnValue = NULL;
        blocks.top()->block = block;
    }
    void popBlock() {
        CodeGenBlock *top = blocks.top();
        blocks.pop();
        delete top;
    }
    void setCurrentReturnValue(Value *value) {
        blocks.top()->returnValue = value;
    }
    Value* getCurrentReturnValue() {
        return blocks.top()->returnValue;
    }
};
