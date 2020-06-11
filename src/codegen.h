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
#include "type.h"
#include "node.h"

// using namespace llvm;

static llvm::LLVMContext MyContext;
static llvm::IRBuilder<> builder{MyContext};

/* type shorthands */
static auto lint64 = llvm::Type::getInt64Ty(MyContext);
static auto lvoid = llvm::Type::getVoidTy(MyContext);
/* constant shorthands */
static auto lzero = llvm::ConstantInt::get(lint64, llvm::APInt(64, 0));
static auto lone = llvm::ConstantInt::get(lint64, llvm::APInt(64, 1));
static auto lnull = llvm::Constant::getNullValue(lint64);

class CodeGenBlock {
public:
    llvm::BasicBlock *block;
    llvm::Value *returnValue;
};

class CodeGenContext {
    std::stack<CodeGenBlock *> blocks;
    llvm::Function *mainFunction;

public:
    llvm::Module *module;
    SymbolTable<llvm::Value> venv;
    SymbolTable<llvm::Type> tenv;
    std::stack<std::tuple<llvm::BasicBlock *, llvm::BasicBlock *>> loopstk; // for break/continue

    CodeGenContext() {
        module = new llvm::Module("main", MyContext);
        tenv.push(Symbol("int"), lint64);
    }

    void generateCode(NExpr *root);
    llvm::GenericValue runCode();

    llvm::Type *castType(Type *type);
    llvm::Function *createIntrinsicFunction(
        const std::string &name,
        const std::vector<llvm::Type *> &args,
        llvm::Type *retType);

    /* Block stack operations */
    llvm::BasicBlock *currentBlock() {
        return blocks.top()->block;
    }
    void pushBlock(llvm::BasicBlock *block) {
        blocks.push(new CodeGenBlock());
        blocks.top()->returnValue = NULL;
        blocks.top()->block = block;
    }
    void popBlock() {
        CodeGenBlock *top = blocks.top();
        blocks.pop();
        delete top;
    }
    void setCurrentReturnValue(llvm::Value *value) {
        blocks.top()->returnValue = value;
    }
    llvm::Value* getCurrentReturnValue() {
        return blocks.top()->returnValue;
    }
};
