#pragma once

#include <napi.h>
#include <llvm/IR/LLVMContext.h>
#include <memory>

namespace llvm_nodejs {

class LLVMContextWrapper : public Napi::ObjectWrap<LLVMContextWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    LLVMContextWrapper(const Napi::CallbackInfo& info);
    
    // Getter for the internal context
    llvm::LLVMContext& GetContext() { return *context_; }

private:
    Napi::Value CreateModule(const Napi::CallbackInfo& info);
    
    std::unique_ptr<llvm::LLVMContext> context_;
};

// Module initialization function
Napi::Object InitAll(Napi::Env env, Napi::Object exports);

}  // namespace llvm_nodejs 