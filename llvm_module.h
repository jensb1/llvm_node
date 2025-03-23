#pragma once

#include <napi.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <memory>

namespace llvm_nodejs {

class ModuleWrapper : public Napi::ObjectWrap<ModuleWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    ModuleWrapper(const Napi::CallbackInfo& info);
    
    // Static method to create a new ModuleWrapper from an existing module
    static Napi::Object Create(Napi::Env env, std::unique_ptr<llvm::Module> module);
    
    // Getter for the internal module
    llvm::Module* GetModule() const { return module_.get(); }

    // Module methods to expose to JavaScript
    Napi::Value GetModuleName(const Napi::CallbackInfo& info);
    Napi::Value SetModuleName(const Napi::CallbackInfo& info);
    Napi::Value Dump(const Napi::CallbackInfo& info);
    Napi::Value SetTargetTriple(const Napi::CallbackInfo& info);
    Napi::Value SetDataLayout(const Napi::CallbackInfo& info);
    Napi::Value CreateFunction(const Napi::CallbackInfo& info);
    Napi::Value Verify(const Napi::CallbackInfo& info);

private:
    std::unique_ptr<llvm::Module> module_;
    
    // Static persistent constructor
    static Napi::FunctionReference constructor;
};

}  // namespace llvm_nodejs 