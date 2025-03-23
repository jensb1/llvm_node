#pragma once

#include <napi.h>
#include <llvm/IR/IRBuilder.h>
#include "llvm_types.h"

namespace llvm_nodejs {

class ArgumentWrapper : public Napi::ObjectWrap<ArgumentWrapper> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object Create(Napi::Env env, llvm::Argument* argument);
    
    ArgumentWrapper(const Napi::CallbackInfo& info);
    llvm::Argument* GetArgument() const { return argument_; }
    
    // Argument methods
    Napi::Value GetName(const Napi::CallbackInfo& info);
    Napi::Value SetName(const Napi::CallbackInfo& info);
    Napi::Value GetType(const Napi::CallbackInfo& info);
    Napi::Value GetParent(const Napi::CallbackInfo& info);
    Napi::Value GetArgNo(const Napi::CallbackInfo& info);
    
private:
    llvm::Argument* argument_;
};

class FunctionWrapper : public Napi::ObjectWrap<FunctionWrapper> {
public:
    static Napi::FunctionReference constructor;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object Create(Napi::Env env, llvm::Function* function);
    
    FunctionWrapper(const Napi::CallbackInfo& info);
    llvm::Function* GetFunction() const { return function_; }
    
    // Function methods
    Napi::Value GetName(const Napi::CallbackInfo& info);
    Napi::Value SetName(const Napi::CallbackInfo& info);
    Napi::Value GetReturnType(const Napi::CallbackInfo& info);
    Napi::Value GetArgumentCount(const Napi::CallbackInfo& info);
    Napi::Value GetArgument(const Napi::CallbackInfo& info);
    Napi::Value CreateBasicBlock(const Napi::CallbackInfo& info);
    Napi::Value GetBasicBlocks(const Napi::CallbackInfo& info);
    Napi::Value Dump(const Napi::CallbackInfo& info);
    
private:
    llvm::Function* function_;
};

}  // namespace llvm_nodejs