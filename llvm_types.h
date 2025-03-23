#pragma once

#include <napi.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>

namespace llvm_nodejs {

class TypeWrapper : public Napi::ObjectWrap<TypeWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object Create(Napi::Env env, llvm::Type* type);
    TypeWrapper(const Napi::CallbackInfo& info);
    
    llvm::Type* GetType() const { return type_; }
    static bool IsInstance(const Napi::Object& obj) {
        return obj.InstanceOf(constructor.Value());
    }

private:
    static Napi::FunctionReference constructor;
    llvm::Type* type_;
    
    Napi::Value IsIntegerTy(const Napi::CallbackInfo& info);
    Napi::Value IsFloatTy(const Napi::CallbackInfo& info);
    Napi::Value IsDoubleTy(const Napi::CallbackInfo& info);
    Napi::Value IsPointerTy(const Napi::CallbackInfo& info);
    Napi::Value IsStructTy(const Napi::CallbackInfo& info);
    Napi::Value IsArrayTy(const Napi::CallbackInfo& info);
    Napi::Value IsVoidTy(const Napi::CallbackInfo& info);
    Napi::Value Dump(const Napi::CallbackInfo& info);
};

class StructTypeWrapper : public Napi::ObjectWrap<StructTypeWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object CreateWrapper(Napi::Env env, llvm::StructType* structType);
    static Napi::Value Create(const Napi::CallbackInfo& info);
    StructTypeWrapper(const Napi::CallbackInfo& info);
    static bool IsInstance(const Napi::Object& obj) {
        return obj.InstanceOf(constructor.Value());
    }
    
    llvm::StructType* GetStructType() const { return structType_; }

private:
    static Napi::FunctionReference constructor;
    llvm::StructType* structType_;
    
    Napi::Value SetBody(const Napi::CallbackInfo& info);
    Napi::Value GetName(const Napi::CallbackInfo& info);
    Napi::Value GetNumElements(const Napi::CallbackInfo& info);
    Napi::Value GetElementType(const Napi::CallbackInfo& info);
    Napi::Value Dump(const Napi::CallbackInfo& info);
};

class ArrayTypeWrapper : public Napi::ObjectWrap<ArrayTypeWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object CreateWrapper(Napi::Env env, llvm::ArrayType* arrayType);
    static Napi::Value Get(const Napi::CallbackInfo& info);
    ArrayTypeWrapper(const Napi::CallbackInfo& info);
    
    llvm::ArrayType* GetArrayType() const { return arrayType_; }

private:
    static Napi::FunctionReference constructor;
    llvm::ArrayType* arrayType_;
    
    Napi::Value GetElementType(const Napi::CallbackInfo& info);
    Napi::Value GetNumElements(const Napi::CallbackInfo& info);
    Napi::Value Dump(const Napi::CallbackInfo& info);
};

class PointerTypeWrapper : public Napi::ObjectWrap<PointerTypeWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object CreateWrapper(Napi::Env env, llvm::PointerType* pointerType);
    static Napi::Value Get(const Napi::CallbackInfo& info);
    PointerTypeWrapper(const Napi::CallbackInfo& info);
    
    llvm::PointerType* GetPointerType() const { return pointerType_; }

private:
    static Napi::FunctionReference constructor;
    llvm::PointerType* pointerType_;
    
    Napi::Value GetElementType(const Napi::CallbackInfo& info);
    Napi::Value GetAddressSpace(const Napi::CallbackInfo& info);
    Napi::Value Dump(const Napi::CallbackInfo& info);
};

class FunctionTypeWrapper : public Napi::ObjectWrap<FunctionTypeWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object CreateWrapper(Napi::Env env, llvm::FunctionType* functionType);
    static Napi::FunctionReference constructor;
    
    FunctionTypeWrapper(const Napi::CallbackInfo& info);
    llvm::FunctionType* GetFunctionType() { return functionType_; }
    
private:
    static Napi::Value Get(const Napi::CallbackInfo& info);
    Napi::Value GetReturnType(const Napi::CallbackInfo& info);
    Napi::Value GetParamTypes(const Napi::CallbackInfo& info);
    Napi::Value IsVarArg(const Napi::CallbackInfo& info);
    Napi::Value GetNumParams(const Napi::CallbackInfo& info);
    Napi::Value Dump(const Napi::CallbackInfo& info);
    
    llvm::FunctionType* functionType_;
};


// Function to initialize all type wrappers
Napi::Object InitTypes(Napi::Env env, Napi::Object exports);

}  // namespace llvm_nodejs