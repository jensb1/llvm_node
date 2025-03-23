#include <napi.h>

#include "llvm_function.h"
#include "llvm_builder.h"
#include <llvm/IR/Function.h>
#include <iostream>

namespace llvm_nodejs {

// Initialize the static constructor reference
Napi::FunctionReference FunctionWrapper::constructor;
Napi::FunctionReference ArgumentWrapper::constructor;

// FunctionWrapper implementation
FunctionWrapper::FunctionWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<FunctionWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        function_ = static_cast<llvm::Function*>(
            info[0].As<Napi::External<llvm::Function>>().Data());
    } else {
        Napi::TypeError::New(env, "FunctionWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

Napi::Object FunctionWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Function", {
        InstanceMethod("getName", &FunctionWrapper::GetName),
        InstanceMethod("setName", &FunctionWrapper::SetName),
        InstanceMethod("getReturnType", &FunctionWrapper::GetReturnType),
        InstanceMethod("getArgumentCount", &FunctionWrapper::GetArgumentCount),
        InstanceMethod("getArgument", &FunctionWrapper::GetArgument),
        InstanceMethod("createBasicBlock", &FunctionWrapper::CreateBasicBlock),
        InstanceMethod("getBasicBlocks", &FunctionWrapper::GetBasicBlocks),
        InstanceMethod("dump", &FunctionWrapper::Dump)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Function", func);
    return exports;
}

Napi::Object FunctionWrapper::Create(Napi::Env env, llvm::Function* function) {
    Napi::External<llvm::Function> external = Napi::External<llvm::Function>::New(env, function);
    return constructor.New({ external });
}

Napi::Value FunctionWrapper::GetName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, function_->getName().str());
}

Napi::Value FunctionWrapper::SetName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Expected string argument").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string name = info[0].As<Napi::String>().Utf8Value();
    function_->setName(name);
    
    return env.Undefined();
}

Napi::Value FunctionWrapper::GetReturnType(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    llvm::Type* returnType = function_->getReturnType();
    return TypeWrapper::Create(env, returnType);
}

Napi::Value FunctionWrapper::GetArgumentCount(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, function_->arg_size());
}

Napi::Value FunctionWrapper::GetArgument(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Expected index argument").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    unsigned index = info[0].As<Napi::Number>().Uint32Value();
    if (index >= function_->arg_size()) {
        Napi::RangeError::New(env, "Argument index out of range").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    // Get the argument at the specified index
    llvm::Argument* arg = function_->getArg(index);
    
    // Return a proper ArgumentWrapper instead of a simple object
    return ArgumentWrapper::Create(env, arg);
}

Napi::Value FunctionWrapper::CreateBasicBlock(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    std::string name = "";
    if (info.Length() >= 1 && info[0].IsString()) {
        name = info[0].As<Napi::String>().Utf8Value();
    }
    
    std::cout << "Creating basic block with name: " << name << std::endl;
    llvm::LLVMContext& context = function_->getContext();
    std::cout << "Context: " << &context << std::endl;
    llvm::BasicBlock* block = llvm::BasicBlock::Create(context, name, function_);
    
    // Return a proper BasicBlockWrapper instead of a simple object
    return BasicBlockWrapper::Create(env, block);
}

Napi::Value FunctionWrapper::GetBasicBlocks(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    Napi::Array blocks = Napi::Array::New(env);
    unsigned i = 0;
    
    for (auto& block : *function_) {
        blocks[i++] = BasicBlockWrapper::Create(env, &block);
    }
    
    return blocks;
}


Napi::Value FunctionWrapper::Dump(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string str;
    llvm::raw_string_ostream stream(str);
    function_->print(stream);
    return Napi::String::New(env, str);
}

// ArgumentWrapper implementation
ArgumentWrapper::ArgumentWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<ArgumentWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        argument_ = static_cast<llvm::Argument*>(
            info[0].As<Napi::External<llvm::Argument>>().Data());
    } else {
        Napi::TypeError::New(env, "ArgumentWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

Napi::Object ArgumentWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Argument", {
        InstanceMethod("getName", &ArgumentWrapper::GetName),
        InstanceMethod("setName", &ArgumentWrapper::SetName),
        InstanceMethod("getType", &ArgumentWrapper::GetType),
        InstanceMethod("getParent", &ArgumentWrapper::GetParent),
        InstanceMethod("getArgNo", &ArgumentWrapper::GetArgNo)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Argument", func);
    return exports;
}

Napi::Object ArgumentWrapper::Create(Napi::Env env, llvm::Argument* argument) {
    Napi::External<llvm::Argument> external = Napi::External<llvm::Argument>::New(env, argument);
    return constructor.New({ external });
}

Napi::Value ArgumentWrapper::GetName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (argument_->hasName()) {
        return Napi::String::New(env, argument_->getName().str());
    }
    return env.Null();
}

Napi::Value ArgumentWrapper::SetName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Expected string argument").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string name = info[0].As<Napi::String>().Utf8Value();
    argument_->setName(name);
    
    return env.Undefined();
}

Napi::Value ArgumentWrapper::GetType(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    llvm::Type* type = argument_->getType();
    return TypeWrapper::Create(env, type);
}

Napi::Value ArgumentWrapper::GetParent(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    llvm::Function* parent = argument_->getParent();
    return FunctionWrapper::Create(env, parent);
}

Napi::Value ArgumentWrapper::GetArgNo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, argument_->getArgNo());
}

}  // namespace llvm_nodejs