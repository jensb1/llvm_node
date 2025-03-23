#include "llvm_context.h"
#include "llvm_module.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace llvm_nodejs {

LLVMContextWrapper::LLVMContextWrapper(const Napi::CallbackInfo& info) 
    : Napi::ObjectWrap<LLVMContextWrapper>(info) {
    context_ = std::make_unique<llvm::LLVMContext>();
}

Napi::Object LLVMContextWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LLVMContext", {
        InstanceMethod("createModule", &LLVMContextWrapper::CreateModule)
    });

    exports.Set("LLVMContext", func);
    return exports;
}

Napi::Value LLVMContextWrapper::CreateModule(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Module name must be a string")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string name = info[0].As<Napi::String>().Utf8Value();
    
    // Create a new LLVM module
    std::unique_ptr<llvm::Module> module = 
        std::make_unique<llvm::Module>(name, *context_);
    
    // Create and return a ModuleWrapper object
    return ModuleWrapper::Create(env, std::move(module));
}



}  // namespace llvm_nodejs
