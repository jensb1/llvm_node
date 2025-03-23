#pragma once

#include <napi.h>
#include <llvm/IR/IRBuilder.h>
#include "llvm_types.h"

namespace llvm_nodejs {

class IRBuilderWrapper : public Napi::ObjectWrap<IRBuilderWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object Create(Napi::Env env, llvm::IRBuilder<>* builder);
    
    IRBuilderWrapper(const Napi::CallbackInfo& info);
    ~IRBuilderWrapper();
    
    llvm::IRBuilder<>* GetBuilder() { return builder_; }

    static llvm::Value* UnwrapValue(const Napi::Value& value);
private:
    static Napi::FunctionReference constructor;
    
    // Helper methods
    static Napi::Value WrapValue(Napi::Env env, llvm::Value* value);
    
    // IRBuilder methods
    Napi::Value CreateRetVoid(const Napi::CallbackInfo& info);
    Napi::Value CreateRet(const Napi::CallbackInfo& info);
    Napi::Value CreateAdd(const Napi::CallbackInfo& info);
    Napi::Value CreateSub(const Napi::CallbackInfo& info);
    Napi::Value CreateMul(const Napi::CallbackInfo& info);
    Napi::Value CreateICmpEQ(const Napi::CallbackInfo& info);
    Napi::Value CreateICmpNE(const Napi::CallbackInfo& info);
    Napi::Value CreateICmpSLT(const Napi::CallbackInfo& info);
    Napi::Value CreateICmpSGT(const Napi::CallbackInfo& info);
    Napi::Value CreateBr(const Napi::CallbackInfo& info);
    Napi::Value CreateCondBr(const Napi::CallbackInfo& info);
    Napi::Value SetInsertPoint(const Napi::CallbackInfo& info);
    Napi::Value GetInsertBlock(const Napi::CallbackInfo& info);
    Napi::Value CreateStructGEP(const Napi::CallbackInfo& info);
    // Add the missing method declarations
    Napi::Value CreateAlloca(const Napi::CallbackInfo& info);
    Napi::Value CreateLoad(const Napi::CallbackInfo& info);
    Napi::Value CreateStore(const Napi::CallbackInfo& info);
    Napi::Value CreateCall(const Napi::CallbackInfo& info);
    Napi::Value CreateGEP(const Napi::CallbackInfo& info);
    Napi::Value CreatePHI(const Napi::CallbackInfo& info);
    
    llvm::IRBuilder<>* builder_ = nullptr;
};

// Define the wrapper classes before using their static members
// Base Value wrapper class
class ValueWrapper : public Napi::ObjectWrap<ValueWrapper> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object Create(Napi::Env env, llvm::Value* value);
    
    ValueWrapper(const Napi::CallbackInfo& info);
    llvm::Value* GetValue() const { return value_; }
    
private:
    llvm::Value* value_;
};

// Constant wrapper class
class ConstantWrapper : public Napi::ObjectWrap<ConstantWrapper> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object Create(Napi::Env env, llvm::Constant* constant);
    
    ConstantWrapper(const Napi::CallbackInfo& info);
    llvm::Constant* GetConstant() const { return constant_; }
    
private:
    llvm::Constant* constant_;
};

// Instruction wrapper class
class InstructionWrapper : public Napi::ObjectWrap<InstructionWrapper> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object Create(Napi::Env env, llvm::Instruction* instruction);
    
    InstructionWrapper(const Napi::CallbackInfo& info);
    llvm::Instruction* GetInstruction() const { return instruction_; }
    
private:
    llvm::Instruction* instruction_;
};

// BasicBlock wrapper class
class BasicBlockWrapper : public Napi::ObjectWrap<BasicBlockWrapper> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object Create(Napi::Env env, llvm::BasicBlock* basicBlock);
    
    BasicBlockWrapper(const Napi::CallbackInfo& info);
    llvm::BasicBlock* GetBasicBlock() const { return basicBlock_; }
    
    // Add static method to create a basic block
    static Napi::Value CreateBasicBlock(const Napi::CallbackInfo& info);
    
private:
    llvm::BasicBlock* basicBlock_;
};

// PHINode wrapper class
class PHINodeWrapper : public Napi::ObjectWrap<PHINodeWrapper> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object Create(Napi::Env env, llvm::PHINode* phiNode);
    
    PHINodeWrapper(const Napi::CallbackInfo& info);
    llvm::PHINode* GetPHINode() const { return phiNode_; }
    
    // Methods for PHINode
    Napi::Value AddIncoming(const Napi::CallbackInfo& info);
    
private:
    llvm::PHINode* phiNode_;
};








} // namespace llvm_nodejs