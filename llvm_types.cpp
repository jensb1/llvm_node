#include "llvm_types.h"
#include "llvm_context.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <iostream>
namespace llvm_nodejs {

// Initialize static constructor references
Napi::FunctionReference TypeWrapper::constructor;
Napi::FunctionReference StructTypeWrapper::constructor;
Napi::FunctionReference ArrayTypeWrapper::constructor;
Napi::FunctionReference PointerTypeWrapper::constructor;
Napi::FunctionReference FunctionTypeWrapper::constructor;

//
// Base TypeWrapper implementation
//
TypeWrapper::TypeWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<TypeWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        type_ = static_cast<llvm::Type*>(info[0].As<Napi::External<llvm::Type>>().Data());
    } else {
        Napi::TypeError::New(env, "TypeWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

Napi::Object TypeWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Type", {
        InstanceMethod("isIntegerTy", &TypeWrapper::IsIntegerTy),
        InstanceMethod("isFloatTy", &TypeWrapper::IsFloatTy),
        InstanceMethod("isDoubleTy", &TypeWrapper::IsDoubleTy),
        InstanceMethod("isPointerTy", &TypeWrapper::IsPointerTy),
        InstanceMethod("isStructTy", &TypeWrapper::IsStructTy),
        InstanceMethod("isArrayTy", &TypeWrapper::IsArrayTy),
        InstanceMethod("isVoidTy", &TypeWrapper::IsVoidTy),
        InstanceMethod("dump", &TypeWrapper::Dump)
    });


    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Type", func);
    return exports;
}

Napi::Object TypeWrapper::Create(Napi::Env env, llvm::Type* type) {
    Napi::External<llvm::Type> external = Napi::External<llvm::Type>::New(env, type);
    return constructor.New({ external });
}

Napi::Value TypeWrapper::IsIntegerTy(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), type_->isIntegerTy());
}

Napi::Value TypeWrapper::IsFloatTy(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), type_->isFloatTy());
}

Napi::Value TypeWrapper::IsDoubleTy(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), type_->isDoubleTy());
}

Napi::Value TypeWrapper::IsPointerTy(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), type_->isPointerTy());
}

Napi::Value TypeWrapper::IsStructTy(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), type_->isStructTy());
}

Napi::Value TypeWrapper::IsArrayTy(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), type_->isArrayTy());
}

Napi::Value TypeWrapper::IsVoidTy(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), type_->isVoidTy());
}

Napi::Value TypeWrapper::Dump(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string str;
    llvm::raw_string_ostream stream(str);
    type_->print(stream);
    return Napi::String::New(env, str);
}

//
// StructTypeWrapper implementation
//
StructTypeWrapper::StructTypeWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<StructTypeWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        structType_ = static_cast<llvm::StructType*>(
            info[0].As<Napi::External<llvm::StructType>>().Data());
    } else {
        Napi::TypeError::New(env, "StructTypeWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

Napi::Object StructTypeWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "StructType", {
        InstanceMethod("setBody", &StructTypeWrapper::SetBody),
        InstanceMethod("getName", &StructTypeWrapper::GetName),
        InstanceMethod("getNumElements", &StructTypeWrapper::GetNumElements),
        InstanceMethod("dump", &StructTypeWrapper::Dump),
        StaticMethod("create", &StructTypeWrapper::Create)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("StructType", func);
    return exports;
}

Napi::Object StructTypeWrapper::CreateWrapper(Napi::Env env, llvm::StructType* structType) {
    Napi::External<llvm::StructType> external = Napi::External<llvm::StructType>::New(env, structType);
    return constructor.New({ external });
}

Napi::Value StructTypeWrapper::Create(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsString()) {
        Napi::TypeError::New(env, "Expected context and name arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    
    LLVMContextWrapper* contextWrapper = 
        Napi::ObjectWrap<LLVMContextWrapper>::Unwrap(info[0].As<Napi::Object>());
    std::string name = info[1].As<Napi::String>().Utf8Value();
    
    llvm::StructType* structType = llvm::StructType::create(
        contextWrapper->GetContext(), name);
    
    return CreateWrapper(env, structType);
}

Napi::Value StructTypeWrapper::SetBody(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsArray()) {
        Napi::TypeError::New(env, "Expected array of types")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    Napi::Array typesArray = info[0].As<Napi::Array>();
    std::vector<llvm::Type*> types;
    
    for (uint32_t i = 0; i < typesArray.Length(); i++) {
        Napi::Value val = typesArray[i];
        if (!val.IsObject()) {
            Napi::TypeError::New(env, "Array must contain Type objects")
                .ThrowAsJavaScriptException();
            return env.Undefined();
        }
        
        TypeWrapper* typeWrapper = Napi::ObjectWrap<TypeWrapper>::Unwrap(val.As<Napi::Object>());
        types.push_back(typeWrapper->GetType());
    }
    
    structType_->setBody(types);
    return env.Undefined();
}

Napi::Value StructTypeWrapper::GetName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (structType_->hasName()) {
        return Napi::String::New(env, structType_->getName().str());
    }
    return env.Null();
}

Napi::Value StructTypeWrapper::GetNumElements(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), structType_->getNumElements());
}


Napi::Value StructTypeWrapper::Dump(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string str;
    llvm::raw_string_ostream stream(str);
    structType_->print(stream);
    return Napi::String::New(env, str);
}

//
// ArrayTypeWrapper implementation
//
ArrayTypeWrapper::ArrayTypeWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<ArrayTypeWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        arrayType_ = static_cast<llvm::ArrayType*>(
            info[0].As<Napi::External<llvm::ArrayType>>().Data());
    } else {
        Napi::TypeError::New(env, "ArrayTypeWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

Napi::Object ArrayTypeWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ArrayType", {
        InstanceMethod("getNumElements", &ArrayTypeWrapper::GetNumElements),
        InstanceMethod("dump", &ArrayTypeWrapper::Dump),
        StaticMethod("get", &ArrayTypeWrapper::Get)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("ArrayType", func);
    return exports;
}

Napi::Object ArrayTypeWrapper::CreateWrapper(Napi::Env env, llvm::ArrayType* arrayType) {
    Napi::External<llvm::ArrayType> external = Napi::External<llvm::ArrayType>::New(env, arrayType);
    return constructor.New({ external });
}

Napi::Value ArrayTypeWrapper::Get(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Expected element type and size arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    
    TypeWrapper* elementTypeWrapper = Napi::ObjectWrap<TypeWrapper>::Unwrap(info[0].As<Napi::Object>());
    uint32_t numElements = info[1].As<Napi::Number>().Uint32Value();
    
    llvm::ArrayType* arrayType = llvm::ArrayType::get(
        elementTypeWrapper->GetType(), numElements);
    
    return CreateWrapper(env, arrayType);
}


Napi::Value ArrayTypeWrapper::GetNumElements(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), arrayType_->getNumElements());
}

Napi::Value ArrayTypeWrapper::Dump(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string str;
    llvm::raw_string_ostream stream(str);
    arrayType_->print(stream);
    return Napi::String::New(env, str);
}

//
// PointerTypeWrapper implementation
//
PointerTypeWrapper::PointerTypeWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<PointerTypeWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        pointerType_ = static_cast<llvm::PointerType*>(
            info[0].As<Napi::External<llvm::PointerType>>().Data());
    } else {
        Napi::TypeError::New(env, "PointerTypeWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

Napi::Object PointerTypeWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "PointerType", {
        InstanceMethod("getAddressSpace", &PointerTypeWrapper::GetAddressSpace),
        InstanceMethod("dump", &PointerTypeWrapper::Dump),
        StaticMethod("get", &PointerTypeWrapper::Get)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("PointerType", func);
    return exports;
}

Napi::Object PointerTypeWrapper::CreateWrapper(Napi::Env env, llvm::PointerType* pointerType) {
    Napi::External<llvm::PointerType> external = Napi::External<llvm::PointerType>::New(env, pointerType);
    return constructor.New({ external });
}

Napi::Value PointerTypeWrapper::Get(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Expected element type and address space arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    
    TypeWrapper* elementTypeWrapper = Napi::ObjectWrap<TypeWrapper>::Unwrap(info[0].As<Napi::Object>());
    unsigned addressSpace = info[1].As<Napi::Number>().Uint32Value();
    
    llvm::PointerType* pointerType = llvm::PointerType::get(
        elementTypeWrapper->GetType(), addressSpace);
    
    return CreateWrapper(env, pointerType);
}


Napi::Value PointerTypeWrapper::GetAddressSpace(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), pointerType_->getAddressSpace());
}

Napi::Value PointerTypeWrapper::Dump(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string str;
    llvm::raw_string_ostream stream(str);
    pointerType_->print(stream);
    return Napi::String::New(env, str);
}

//
// FunctionTypeWrapper implementation
//
FunctionTypeWrapper::FunctionTypeWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<FunctionTypeWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        functionType_ = static_cast<llvm::FunctionType*>(
            info[0].As<Napi::External<llvm::FunctionType>>().Data());
    } else {
        Napi::TypeError::New(env, "FunctionTypeWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

Napi::Object FunctionTypeWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "FunctionType", {
        InstanceMethod("getReturnType", &FunctionTypeWrapper::GetReturnType),
        InstanceMethod("getParamTypes", &FunctionTypeWrapper::GetParamTypes),
        InstanceMethod("getNumParams", &FunctionTypeWrapper::GetNumParams),
        InstanceMethod("isVarArg", &FunctionTypeWrapper::IsVarArg),
        InstanceMethod("dump", &FunctionTypeWrapper::Dump),
        StaticMethod("get", &FunctionTypeWrapper::Get)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("FunctionType", func);
    return exports;
}

Napi::Object FunctionTypeWrapper::CreateWrapper(Napi::Env env, llvm::FunctionType* functionType) {
    Napi::External<llvm::FunctionType> external = Napi::External<llvm::FunctionType>::New(env, functionType);
    return constructor.New({ external });
}

Napi::Value FunctionTypeWrapper::Get(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsArray()) {
        Napi::TypeError::New(env, "Expected return type and parameter types array")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    
    TypeWrapper* returnTypeWrapper = Napi::ObjectWrap<TypeWrapper>::Unwrap(info[0].As<Napi::Object>());
    Napi::Array paramTypesArray = info[1].As<Napi::Array>();
    
    std::vector<llvm::Type*> paramTypes;
    for (uint32_t i = 0; i < paramTypesArray.Length(); i++) {
        Napi::Value val = paramTypesArray[i];
        if (!val.IsObject()) {
            Napi::TypeError::New(env, "Parameter types array must contain Type objects")
                .ThrowAsJavaScriptException();
            return env.Null();
        }
        
        TypeWrapper* typeWrapper = Napi::ObjectWrap<TypeWrapper>::Unwrap(val.As<Napi::Object>());
        paramTypes.push_back(typeWrapper->GetType());
    }
    
    bool isVarArg = false;
    if (info.Length() >= 3 && info[2].IsBoolean()) {
        isVarArg = info[2].As<Napi::Boolean>().Value();
    }
    
    llvm::FunctionType* functionType = llvm::FunctionType::get(
        returnTypeWrapper->GetType(), paramTypes, isVarArg);
    
    return CreateWrapper(env, functionType);
}

Napi::Value FunctionTypeWrapper::GetReturnType(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    llvm::Type* returnType = functionType_->getReturnType();
    return TypeWrapper::Create(env, returnType);
}

Napi::Value FunctionTypeWrapper::GetParamTypes(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Expected parameter index")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    
    unsigned index = info[0].As<Napi::Number>().Uint32Value();
    if (index >= functionType_->getNumParams()) {
        Napi::RangeError::New(env, "Parameter index out of range")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    
    llvm::Type* paramType = functionType_->getParamType(index);
    return TypeWrapper::Create(env, paramType);
}

Napi::Value FunctionTypeWrapper::GetNumParams(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), functionType_->getNumParams());
}

Napi::Value FunctionTypeWrapper::IsVarArg(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), functionType_->isVarArg());
}

Napi::Value FunctionTypeWrapper::Dump(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string str;
    llvm::raw_string_ostream stream(str);
    functionType_->print(stream);
    return Napi::String::New(env, str);
}

// Update the InitAll function to include the type wrappers
Napi::Object InitTypes(Napi::Env env, Napi::Object exports) {
    std::cout << "Initializing LLVM Types" << std::endl;
    TypeWrapper::Init(env, exports);
    std::cout << "Initialized TypeWrapper" << std::endl;
    StructTypeWrapper::Init(env, exports);
    std::cout << "Initialized StructTypeWrapper" << std::endl;
    ArrayTypeWrapper::Init(env, exports);
    std::cout << "Initialized ArrayTypeWrapper" << std::endl;
    PointerTypeWrapper::Init(env, exports);
    std::cout << "Initialized PointerTypeWrapper" << std::endl;
    FunctionTypeWrapper::Init(env, exports);
    std::cout << "Initialized FunctionTypeWrapper" << std::endl;
    
    // Get the LLVMContext class from exports instead of instance data
    Napi::Object contextClass = exports.Get("LLVMContext").As<Napi::Object>();
    if (!contextClass.IsFunction()) {
        std::cerr << "LLVMContext not found in exports. Make sure to initialize LLVMContext before types." << std::endl;
        return exports;
    }
    
    Napi::ObjectReference prototype = Napi::Persistent(contextClass.Get("prototype").As<Napi::Object>());
    
    prototype.Set("getInt1Ty", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        LLVMContextWrapper* contextWrapper = Napi::ObjectWrap<LLVMContextWrapper>::Unwrap(info.This().As<Napi::Object>());
        llvm::Type* type = llvm::Type::getInt1Ty(contextWrapper->GetContext());
        return TypeWrapper::Create(env, type);
    }));
    
    prototype.Set("getInt8Ty", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        LLVMContextWrapper* contextWrapper = Napi::ObjectWrap<LLVMContextWrapper>::Unwrap(info.This().As<Napi::Object>());
        llvm::Type* type = llvm::Type::getInt8Ty(contextWrapper->GetContext());
        return TypeWrapper::Create(env, type);
    }));
    
    prototype.Set("getInt32Ty", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        LLVMContextWrapper* contextWrapper = Napi::ObjectWrap<LLVMContextWrapper>::Unwrap(info.This().As<Napi::Object>());
        llvm::Type* type = llvm::Type::getInt32Ty(contextWrapper->GetContext());
        return TypeWrapper::Create(env, type);
    }));
    
    prototype.Set("getInt64Ty", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        LLVMContextWrapper* contextWrapper = Napi::ObjectWrap<LLVMContextWrapper>::Unwrap(info.This().As<Napi::Object>());
        llvm::Type* type = llvm::Type::getInt64Ty(contextWrapper->GetContext());
        return TypeWrapper::Create(env, type);
    }));
    
    prototype.Set("getFloatTy", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        LLVMContextWrapper* contextWrapper = Napi::ObjectWrap<LLVMContextWrapper>::Unwrap(info.This().As<Napi::Object>());
        llvm::Type* type = llvm::Type::getFloatTy(contextWrapper->GetContext());
        return TypeWrapper::Create(env, type);
    }));
    
    prototype.Set("getDoubleTy", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        LLVMContextWrapper* contextWrapper = Napi::ObjectWrap<LLVMContextWrapper>::Unwrap(info.This().As<Napi::Object>());
        llvm::Type* type = llvm::Type::getDoubleTy(contextWrapper->GetContext());
        return TypeWrapper::Create(env, type);
    }));
    
    prototype.Set("getVoidTy", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        LLVMContextWrapper* contextWrapper = Napi::ObjectWrap<LLVMContextWrapper>::Unwrap(info.This().As<Napi::Object>());
        llvm::Type* type = llvm::Type::getVoidTy(contextWrapper->GetContext());
        return TypeWrapper::Create(env, type);
    }));
    
    return exports;
}

}  // namespace llvm_nodejs
