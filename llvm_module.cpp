#include "llvm_module.h"
#include <llvm/IR/Verifier.h>
#include "llvm_types.h"
#include "llvm_function.h"
#include <llvm/Support/raw_ostream.h>
#include "llvm_builder.h"

namespace llvm_nodejs {

// Initialize the static constructor reference
Napi::FunctionReference ModuleWrapper::constructor;

ModuleWrapper::ModuleWrapper(const Napi::CallbackInfo& info) 
    : Napi::ObjectWrap<ModuleWrapper>(info) {
    Napi::Env env = info.Env();
    
    // This constructor should not be called directly from JavaScript
    if (info.Length() == 1 && info[0].IsExternal()) {
        // Unwrap the module pointer passed from Create()
        module_ = std::unique_ptr<llvm::Module>(
            static_cast<llvm::Module*>(info[0].As<Napi::External<llvm::Module>>().Data()));
    } else {
        Napi::TypeError::New(env, "ModuleWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

Napi::Object ModuleWrapper::Create(Napi::Env env, std::unique_ptr<llvm::Module> module) {
    // Create an external reference to pass ownership of the module
    Napi::External<llvm::Module> external = Napi::External<llvm::Module>::New(env, module.release());
    
    // Call the constructor with the external reference
    Napi::Object obj = constructor.New({ external });
    
    return obj;
}

Napi::Object ModuleWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Module", {
        InstanceMethod("getName", &ModuleWrapper::GetModuleName),
        InstanceMethod("setName", &ModuleWrapper::SetModuleName),
        InstanceMethod("dump", &ModuleWrapper::Dump),
        InstanceMethod("setTargetTriple", &ModuleWrapper::SetTargetTriple),
        InstanceMethod("setDataLayout", &ModuleWrapper::SetDataLayout),
        InstanceMethod("createFunction", &ModuleWrapper::CreateFunction),
        InstanceMethod("verify", &ModuleWrapper::Verify)
    });

    // Store the constructor for later use in Create()
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Module", func);
    return exports;
}

Napi::Value ModuleWrapper::GetModuleName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, module_->getName().str());
}

Napi::Value ModuleWrapper::SetModuleName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string name = info[0].As<Napi::String>().Utf8Value();
    module_->setModuleIdentifier(name);
    
    return env.Undefined();
}

Napi::Value ModuleWrapper::Dump(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    // Dump the module to string and return it
    std::string str;
    llvm::raw_string_ostream stream(str);
    module_->print(stream, nullptr);
    
    return Napi::String::New(env, str);
}

Napi::Value ModuleWrapper::SetTargetTriple(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string triple = info[0].As<Napi::String>().Utf8Value();
    module_->setTargetTriple(triple);
    
    return env.Undefined();
}

Napi::Value ModuleWrapper::SetDataLayout(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string layout = info[0].As<Napi::String>().Utf8Value();
    module_->setDataLayout(layout);
    
    return env.Undefined();
}

Napi::Value ModuleWrapper::CreateFunction(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsObject()) {
        Napi::TypeError::New(env, "Expected function name and function type").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string name = info[0].As<Napi::String>().Utf8Value();
    FunctionTypeWrapper* typeWrapper = Napi::ObjectWrap<FunctionTypeWrapper>::Unwrap(info[1].As<Napi::Object>());
    
    llvm::Function* function = llvm::Function::Create(
        typeWrapper->GetFunctionType(),
        llvm::Function::ExternalLinkage,
        name,
        module_.get()
    );
    
    return FunctionWrapper::Create(env, function);
}

Napi::Value ModuleWrapper::Verify(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    
    bool isValid = !llvm::verifyModule(*module_, &errorStream);
    
    Napi::Object result = Napi::Object::New(env);
    result.Set("valid", Napi::Boolean::New(env, isValid));
    
    if (!isValid) {
        errorStream.flush();
        result.Set("error", Napi::String::New(env, errorStr));
    }
    
    return result;

}

}  // namespace llvm_nodejs
