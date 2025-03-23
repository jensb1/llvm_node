#include "llvm_builder.h"
#include "llvm_types.h"
#include "llvm_function.h"
#include "llvm_context.h"
#include "llvm_module.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <iostream>
namespace llvm_nodejs {

// Initialize the static constructor reference for IRBuilderWrapper
Napi::FunctionReference IRBuilderWrapper::constructor;
Napi::FunctionReference PHINodeWrapper::constructor;
Napi::FunctionReference ValueWrapper::constructor;
Napi::FunctionReference ConstantWrapper::constructor;
Napi::FunctionReference InstructionWrapper::constructor;
Napi::FunctionReference BasicBlockWrapper::constructor;

// Implementation of wrapper constructors
ValueWrapper::ValueWrapper(const Napi::CallbackInfo& info) 
    : Napi::ObjectWrap<ValueWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        value_ = static_cast<llvm::Value*>(info[0].As<Napi::External<llvm::Value>>().Data());
    } else {
        Napi::TypeError::New(env, "ValueWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

ConstantWrapper::ConstantWrapper(const Napi::CallbackInfo& info) 
    : Napi::ObjectWrap<ConstantWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        constant_ = static_cast<llvm::Constant*>(info[0].As<Napi::External<llvm::Constant>>().Data());
    } else {
        Napi::TypeError::New(env, "ConstantWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

InstructionWrapper::InstructionWrapper(const Napi::CallbackInfo& info) 
    : Napi::ObjectWrap<InstructionWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        instruction_ = static_cast<llvm::Instruction*>(info[0].As<Napi::External<llvm::Instruction>>().Data());
    } else {
        Napi::TypeError::New(env, "InstructionWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

BasicBlockWrapper::BasicBlockWrapper(const Napi::CallbackInfo& info) 
    : Napi::ObjectWrap<BasicBlockWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        basicBlock_ = static_cast<llvm::BasicBlock*>(info[0].As<Napi::External<llvm::BasicBlock>>().Data());
    } else {
        Napi::TypeError::New(env, "BasicBlockWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

// Static Create methods for wrappers
Napi::Object ValueWrapper::Create(Napi::Env env, llvm::Value* value) {
    Napi::External<llvm::Value> external = Napi::External<llvm::Value>::New(env, value);
    return constructor.New({ external });
}

Napi::Object ConstantWrapper::Create(Napi::Env env, llvm::Constant* constant) {
    Napi::External<llvm::Constant> external = Napi::External<llvm::Constant>::New(env, constant);
    return constructor.New({ external });
}

Napi::Object InstructionWrapper::Create(Napi::Env env, llvm::Instruction* instruction) {
    std::cout << "Inside InstructionWrapper::Create" << std::endl;
    
    if (!instruction) {
        std::cout << "Instruction is null!" << std::endl;
        return Napi::Object::New(env); // Return empty object instead of crashing
    }
    
    try {
        std::cout << "Creating External<llvm::Instruction>" << std::endl;
        Napi::External<llvm::Instruction> external = Napi::External<llvm::Instruction>::New(env, instruction);
        
        std::cout << "Calling constructor.New" << std::endl;
        return constructor.New({ external });
    } catch (const std::exception& e) {
        std::cout << "Exception in InstructionWrapper::Create: " << e.what() << std::endl;
        return Napi::Object::New(env); // Return empty object instead of crashing
    } catch (...) {
        std::cout << "Unknown exception in InstructionWrapper::Create" << std::endl;
        return Napi::Object::New(env); // Return empty object instead of crashing
    }
}

Napi::Object BasicBlockWrapper::Create(Napi::Env env, llvm::BasicBlock* basicBlock) {
    Napi::External<llvm::BasicBlock> external = Napi::External<llvm::BasicBlock>::New(env, basicBlock);
    return constructor.New({ external });
}

// IRBuilderWrapper implementation
IRBuilderWrapper::IRBuilderWrapper(const Napi::CallbackInfo& info) 
    : Napi::ObjectWrap<IRBuilderWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        // Unwrap the builder pointer passed from Create()
        builder_ = static_cast<llvm::IRBuilder<>*>(
            info[0].As<Napi::External<llvm::IRBuilder<>>>().Data());
    } else {
        // Create a new builder with a context
        if (info.Length() < 1 || !info[0].IsObject()) {
            Napi::TypeError::New(env, "LLVMContext argument expected")
                .ThrowAsJavaScriptException();
            return;
        }
        
        // Get the context from the LLVMContextWrapper
        auto contextWrapper = Napi::ObjectWrap<LLVMContextWrapper>::Unwrap(info[0].As<Napi::Object>());
        builder_ = new llvm::IRBuilder<>(contextWrapper->GetContext());
    }
}

IRBuilderWrapper::~IRBuilderWrapper() {
    if (builder_) {
        delete builder_;
        builder_ = nullptr;
    }
}

Napi::Object IRBuilderWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "IRBuilder", {
        InstanceMethod("createRetVoid", &IRBuilderWrapper::CreateRetVoid),
        InstanceMethod("createRet", &IRBuilderWrapper::CreateRet),
        InstanceMethod("createAdd", &IRBuilderWrapper::CreateAdd),
        InstanceMethod("createSub", &IRBuilderWrapper::CreateSub),
        InstanceMethod("createMul", &IRBuilderWrapper::CreateMul),
        InstanceMethod("createICmpEQ", &IRBuilderWrapper::CreateICmpEQ),
        InstanceMethod("createICmpNE", &IRBuilderWrapper::CreateICmpNE),
        InstanceMethod("createICmpSLT", &IRBuilderWrapper::CreateICmpSLT),
        InstanceMethod("createICmpSGT", &IRBuilderWrapper::CreateICmpSGT),
        InstanceMethod("createBr", &IRBuilderWrapper::CreateBr),
        InstanceMethod("createCondBr", &IRBuilderWrapper::CreateCondBr),
        InstanceMethod("setInsertPoint", &IRBuilderWrapper::SetInsertPoint),
        InstanceMethod("getInsertBlock", &IRBuilderWrapper::GetInsertBlock),
        InstanceMethod("createAlloca", &IRBuilderWrapper::CreateAlloca),
        InstanceMethod("createLoad", &IRBuilderWrapper::CreateLoad),
        InstanceMethod("createStore", &IRBuilderWrapper::CreateStore),
        InstanceMethod("createCall", &IRBuilderWrapper::CreateCall),
        InstanceMethod("createGEP", &IRBuilderWrapper::CreateGEP),
        InstanceMethod("createPHI", &IRBuilderWrapper::CreatePHI),
        InstanceMethod("createStructGEP", &IRBuilderWrapper::CreateStructGEP)
    });

    // Store the constructor for later use
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("IRBuilder", func);
    return exports;
}

// Helper function to unwrap LLVM Value from JavaScript object
llvm::Value* IRBuilderWrapper::UnwrapValue(const Napi::Value& value) {
    if (!value.IsObject()) {
        return nullptr;
    }
    
    // Try to unwrap as ValueWrapper
    Napi::Object obj = value.As<Napi::Object>();
    if (obj.InstanceOf(ValueWrapper::constructor.Value())) {
        return Napi::ObjectWrap<ValueWrapper>::Unwrap(obj)->GetValue();
    }

    if (obj.InstanceOf(PHINodeWrapper::constructor.Value())) {
        return Napi::ObjectWrap<PHINodeWrapper>::Unwrap(obj)->GetPHINode();
    }

    if (obj.InstanceOf(InstructionWrapper::constructor.Value())) {
        return Napi::ObjectWrap<InstructionWrapper>::Unwrap(obj)->GetInstruction();
    }
    
    // Try other wrapper types that inherit from Value
    if (obj.InstanceOf(ConstantWrapper::constructor.Value())) {
        return Napi::ObjectWrap<ConstantWrapper>::Unwrap(obj)->GetConstant();
    }
    
    if (obj.InstanceOf(BasicBlockWrapper::constructor.Value())) {
        return Napi::ObjectWrap<BasicBlockWrapper>::Unwrap(obj)->GetBasicBlock();
    }

    if (obj.InstanceOf(ArgumentWrapper::constructor.Value())) {
        return Napi::ObjectWrap<ArgumentWrapper>::Unwrap(obj)->GetArgument();
    }
    
    return nullptr;
}

// Helper function to wrap LLVM Value in a JavaScript object
Napi::Value IRBuilderWrapper::WrapValue(Napi::Env env, llvm::Value* value) {
    if (!value) {
        return env.Null();
    }
    
    // Check the type of value and create the appropriate wrapper
    if (llvm::isa<llvm::Constant>(value)) {
        std::cout << "ConstantWrapper::Create(env, llvm::cast<llvm::Constant>(value))" << std::endl;
        return ConstantWrapper::Create(env, llvm::cast<llvm::Constant>(value));
    }
    
    if (llvm::isa<llvm::Instruction>(value)) {
        std::cout << "InstructionWrapper::constructor before Create: " 
                  << (InstructionWrapper::constructor.IsEmpty() ? "No" : "Yes") << std::endl;
        std::cout << "InstructionWrapper::Create(env, llvm::cast<llvm::Instruction>(value))" << std::endl;
        return InstructionWrapper::Create(env, llvm::cast<llvm::Instruction>(value));
    }
    
    if (llvm::isa<llvm::BasicBlock>(value)) {
        std::cout << "BasicBlockWrapper::Create(env, llvm::cast<llvm::BasicBlock>(value))" << std::endl;
        return BasicBlockWrapper::Create(env, llvm::cast<llvm::BasicBlock>(value));
    }

    if (llvm::isa<llvm::Argument>(value)) {
        std::cout << "ArgumentWrapper::Create(env, llvm::cast<llvm::Argument>(value))" << std::endl;
        return ArgumentWrapper::Create(env, llvm::cast<llvm::Argument>(value));
    }

    
    // Default to generic ValueWrapper
    return ValueWrapper::Create(env, value);
}

Napi::Value IRBuilderWrapper::CreateRetVoid(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    llvm::ReturnInst* ret = builder_->CreateRetVoid();
    return WrapValue(env, ret);
}

Napi::Value IRBuilderWrapper::CreateRet(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Value argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Value* value = UnwrapValue(info[0]);
    if (!value) {
        Napi::TypeError::New(env, "Invalid value")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::ReturnInst* ret = builder_->CreateRet(value);
    return WrapValue(env, ret);
}

Napi::Value IRBuilderWrapper::CreateAdd(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject()) {
        Napi::TypeError::New(env, "Two value arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Value* lhs = UnwrapValue(info[0]);
    llvm::Value* rhs = UnwrapValue(info[1]);
    
    if (!lhs || !rhs) {
        Napi::TypeError::New(env, "Invalid values")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string name = "";
    if (info.Length() > 2 && info[2].IsString()) {
        name = info[2].As<Napi::String>().Utf8Value();
    }

    llvm::Value* result = builder_->CreateAdd(lhs, rhs, name);
    Napi::Value result_value = WrapValue(env, result);

    return result_value;
}

Napi::Value IRBuilderWrapper::CreateSub(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject()) {
        Napi::TypeError::New(env, "Two value arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Value* lhs = UnwrapValue(info[0]);
    llvm::Value* rhs = UnwrapValue(info[1]);
    
    if (!lhs || !rhs) {
        Napi::TypeError::New(env, "Invalid values")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string name = "";
    if (info.Length() > 2 && info[2].IsString()) {
        name = info[2].As<Napi::String>().Utf8Value();
    }
    
    llvm::Value* result = builder_->CreateSub(lhs, rhs, name);
    return WrapValue(env, result);
}

Napi::Value IRBuilderWrapper::CreateMul(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject()) {
        Napi::TypeError::New(env, "Two value arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Value* lhs = UnwrapValue(info[0]);
    llvm::Value* rhs = UnwrapValue(info[1]);
    
    if (!lhs || !rhs) {
        Napi::TypeError::New(env, "Invalid values")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string name = "";
    if (info.Length() > 2 && info[2].IsString()) {
        name = info[2].As<Napi::String>().Utf8Value();
    }
    
    llvm::Value* result = builder_->CreateMul(lhs, rhs, name);
    return WrapValue(env, result);
}

Napi::Value IRBuilderWrapper::CreateICmpEQ(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject()) {
        Napi::TypeError::New(env, "Two value arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Value* lhs = UnwrapValue(info[0]);
    llvm::Value* rhs = UnwrapValue(info[1]);
    
    if (!lhs || !rhs) {
        Napi::TypeError::New(env, "Invalid values")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string name = "";
    if (info.Length() > 2 && info[2].IsString()) {
        name = info[2].As<Napi::String>().Utf8Value();
    }
    
    llvm::Value* result = builder_->CreateICmpEQ(lhs, rhs, name);
    return WrapValue(env, result);
}

Napi::Value IRBuilderWrapper::CreateICmpNE(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject()) {
        Napi::TypeError::New(env, "Two value arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Value* lhs = UnwrapValue(info[0]);
    llvm::Value* rhs = UnwrapValue(info[1]);
    
    if (!lhs || !rhs) {
        Napi::TypeError::New(env, "Invalid values")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string name = "";
    if (info.Length() > 2 && info[2].IsString()) {
        name = info[2].As<Napi::String>().Utf8Value();
    }
    
    llvm::Value* result = builder_->CreateICmpNE(lhs, rhs, name);
    return WrapValue(env, result);
}

Napi::Value IRBuilderWrapper::CreateICmpSLT(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject()) {
        Napi::TypeError::New(env, "Two value arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Value* lhs = UnwrapValue(info[0]);
    llvm::Value* rhs = UnwrapValue(info[1]);
    
    if (!lhs || !rhs) {
        Napi::TypeError::New(env, "Invalid values")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string name = "";
    if (info.Length() > 2 && info[2].IsString()) {
        name = info[2].As<Napi::String>().Utf8Value();
    }
    
    llvm::Value* result = builder_->CreateICmpSLT(lhs, rhs, name);
    return WrapValue(env, result);
}

Napi::Value IRBuilderWrapper::CreateICmpSGT(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject()) {
        Napi::TypeError::New(env, "Two value arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Value* lhs = UnwrapValue(info[0]);
    llvm::Value* rhs = UnwrapValue(info[1]);
    
    if (!lhs || !rhs) {
        Napi::TypeError::New(env, "Invalid values")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    std::string name = "";
    if (info.Length() > 2 && info[2].IsString()) {
        name = info[2].As<Napi::String>().Utf8Value();
    }
    
    llvm::Value* result = builder_->CreateICmpSGT(lhs, rhs, name);
    return WrapValue(env, result);
}

Napi::Value IRBuilderWrapper::CreateBr(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "BasicBlock argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    Napi::Object obj = info[0].As<Napi::Object>();
    if (!obj.InstanceOf(BasicBlockWrapper::constructor.Value())) {
        Napi::TypeError::New(env, "BasicBlock argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::BasicBlock* dest = Napi::ObjectWrap<BasicBlockWrapper>::Unwrap(obj)->GetBasicBlock();
    
    if (!dest) {
        Napi::TypeError::New(env, "Invalid basic block")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::BranchInst* branch = builder_->CreateBr(dest);
    return WrapValue(env, branch);
}

Napi::Value IRBuilderWrapper::CreateCondBr(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 3 || !info[0].IsObject() || !info[1].IsObject() || !info[2].IsObject()) {
        Napi::TypeError::New(env, "Condition and two BasicBlock arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Value* condition = UnwrapValue(info[0]);
    
    Napi::Object trueObj = info[1].As<Napi::Object>();
    Napi::Object falseObj = info[2].As<Napi::Object>();
    
    if (!trueObj.InstanceOf(BasicBlockWrapper::constructor.Value()) || 
        !falseObj.InstanceOf(BasicBlockWrapper::constructor.Value())) {
        Napi::TypeError::New(env, "BasicBlock arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::BasicBlock* trueBlock = Napi::ObjectWrap<BasicBlockWrapper>::Unwrap(trueObj)->GetBasicBlock();
    llvm::BasicBlock* falseBlock = Napi::ObjectWrap<BasicBlockWrapper>::Unwrap(falseObj)->GetBasicBlock();
    
    if (!condition || !trueBlock || !falseBlock) {
        Napi::TypeError::New(env, "Invalid condition or basic blocks")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::BranchInst* branch = builder_->CreateCondBr(condition, trueBlock, falseBlock);
    return WrapValue(env, branch);
}

Napi::Value IRBuilderWrapper::SetInsertPoint(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "BasicBlock argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    Napi::Object obj = info[0].As<Napi::Object>();
    if (!obj.InstanceOf(BasicBlockWrapper::constructor.Value())) {
        Napi::TypeError::New(env, "BasicBlock argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::BasicBlock* block = Napi::ObjectWrap<BasicBlockWrapper>::Unwrap(obj)->GetBasicBlock();
    
    if (!block) {
        Napi::TypeError::New(env, "Invalid basic block")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    builder_->SetInsertPoint(block);
    return env.Undefined();
}

Napi::Value IRBuilderWrapper::GetInsertBlock(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    llvm::BasicBlock* block = builder_->GetInsertBlock();
    if (!block) {
        return env.Null();
    }
    
    return BasicBlockWrapper::Create(env, block);
}

// Add more builder methods for completeness
Napi::Value IRBuilderWrapper::CreateAlloca(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Type argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    Napi::Object typeObj = info[0].As<Napi::Object>();
    if (!TypeWrapper::IsInstance(typeObj)) {
        Napi::TypeError::New(env, "Type argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Type* type = Napi::ObjectWrap<TypeWrapper>::Unwrap(typeObj)->GetType();
    
    std::string name = "";
    if (info.Length() > 1 && info[1].IsString()) {
        name = info[1].As<Napi::String>().Utf8Value();
    }
    
    llvm::AllocaInst* alloca = builder_->CreateAlloca(type, nullptr, name);
    return WrapValue(env, alloca);
}

Napi::Value IRBuilderWrapper::CreateLoad(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Value argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }

    // unwrap type
    Napi::Object typeObj = info[0].As<Napi::Object>();
    if (!TypeWrapper::IsInstance(typeObj)) {
        Napi::TypeError::New(env, "Type argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    llvm::Type *type = Napi::ObjectWrap<TypeWrapper>::Unwrap(typeObj)->GetType();

    // unwrap value
    llvm::Value* value = UnwrapValue(info[1]);
    if (!value) {
        Napi::TypeError::New(env, "Invalid value")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }

    llvm::LoadInst* load = builder_->CreateLoad(type, value);
    return WrapValue(env, load);
}

Napi::Value IRBuilderWrapper::CreateStore(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject()) {
        Napi::TypeError::New(env, "Value and pointer arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Value* value = UnwrapValue(info[0]);
    llvm::Value* ptr = UnwrapValue(info[1]);
    
    if (!value || !ptr) {
        Napi::TypeError::New(env, "Invalid value or pointer")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::StoreInst* store = builder_->CreateStore(value, ptr);
    return WrapValue(env, store);
}

Napi::Value IRBuilderWrapper::CreateCall(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsArray()) {
        Napi::TypeError::New(env, "Function and arguments array expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Function* function = nullptr;
    Napi::Object funcObj = info[0].As<Napi::Object>();
    
    if (funcObj.InstanceOf(FunctionWrapper::constructor.Value())) {
        function = Napi::ObjectWrap<FunctionWrapper>::Unwrap(funcObj)->GetFunction();
    } else {
        llvm::Value* funcValue = UnwrapValue(info[0]);
        if (llvm::isa<llvm::Function>(funcValue)) {
            function = llvm::cast<llvm::Function>(funcValue);
        }
    }
    
    if (!function) {
        Napi::TypeError::New(env, "Invalid function")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    Napi::Array argsArray = info[1].As<Napi::Array>();
    std::vector<llvm::Value*> args;
    
    for (uint32_t i = 0; i < argsArray.Length(); i++) {
        Napi::Value arg = argsArray[i];
        llvm::Value* argValue = UnwrapValue(arg);
        if (!argValue) {
            Napi::TypeError::New(env, "Invalid argument at index " + std::to_string(i))
                .ThrowAsJavaScriptException();
            return env.Undefined();
        }
        args.push_back(argValue);
    }
    
    std::string name = "";
    if (info.Length() > 2 && info[2].IsString()) {
        name = info[2].As<Napi::String>().Utf8Value();
    }
    
    llvm::CallInst* call = builder_->CreateCall(function, args, name);
    return WrapValue(env, call);
}

// Additional methods implementation
Napi::Value IRBuilderWrapper::CreateGEP(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsArray()) {
        Napi::TypeError::New(env, "Pointer and indices array expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    llvm::Value* ptr = UnwrapValue(info[0]);
    if (!ptr) {
        Napi::TypeError::New(env, "Invalid pointer")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    Napi::Array indicesArray = info[1].As<Napi::Array>();
    std::vector<llvm::Value*> indices;
    
    for (uint32_t i = 0; i < indicesArray.Length(); i++) {
        Napi::Value idx = indicesArray[i];
        llvm::Value* idxValue = UnwrapValue(idx);
        if (!idxValue) {
            Napi::TypeError::New(env, "Invalid index at position " + std::to_string(i))
                .ThrowAsJavaScriptException();
            return env.Undefined();
        }
        indices.push_back(idxValue);
    }
    
    std::string name = "";
    if (info.Length() > 2 && info[2].IsString()) {
        name = info[2].As<Napi::String>().Utf8Value();
    }
    
    llvm::Value* gep = builder_->CreateGEP(ptr->getType(), ptr, indices);
    return WrapValue(env, gep);
}

Napi::Value IRBuilderWrapper::CreatePHI(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Type and number of reserved values expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    // Get the type
    Napi::Object typeObj = info[0].As<Napi::Object>();
    if (!TypeWrapper::IsInstance(typeObj)) {
        Napi::TypeError::New(env, "Type argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    llvm::Type* type = Napi::ObjectWrap<TypeWrapper>::Unwrap(typeObj)->GetType();
    
    // Get the number of reserved values
    unsigned numReservedValues = info[1].As<Napi::Number>().Uint32Value();
    
    // Get the name (optional)
    std::string name = "";
    if (info.Length() > 2 && info[2].IsString()) {
        name = info[2].As<Napi::String>().Utf8Value();
    }
    
    llvm::PHINode* phi = builder_->CreatePHI(type, numReservedValues, name);
    return PHINodeWrapper::Create(env, phi);
}

Napi::Value IRBuilderWrapper::CreateStructGEP(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 3 || !info[0].IsObject() || !info[1].IsObject() || !info[2].IsNumber()) {
        Napi::TypeError::New(env, "Type, pointer, and index arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    // Get the type
    Napi::Object typeObj = info[0].As<Napi::Object>();
    if (!StructTypeWrapper::IsInstance(typeObj)) {
        Napi::TypeError::New(env, "Type argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    llvm::Type* type = Napi::ObjectWrap<TypeWrapper>::Unwrap(typeObj)->GetType();
    
    // Get the pointer
    llvm::Value* ptr = UnwrapValue(info[1]);
    if (!ptr) {
        Napi::TypeError::New(env, "Invalid pointer")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    // Get the index
    unsigned idx = info[2].As<Napi::Number>().Uint32Value();
    
    // Get the name (optional)
    std::string name = "";
    if (info.Length() > 3 && info[3].IsString()) {
        name = info[3].As<Napi::String>().Utf8Value();
    }
    
    // Create the struct GEP instruction
    llvm::Value* gep = builder_->CreateStructGEP(type, ptr, idx, name);
    return WrapValue(env, gep);
}

// Replace InitValueWrappers with individual Init functions for each wrapper class
Napi::Object ValueWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Value", {
        // Add methods as needed
    });
    
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    
    exports.Set("Value", func);
    return exports;
}

Napi::Object ConstantWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Constant", {
        // Add methods as needed
    });
    
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    
    exports.Set("Constant", func);
    return exports;
}

Napi::Object InstructionWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Instruction", {
        // Add methods as needed
    });
    
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    
    exports.Set("Instruction", func);
    return exports;
}


// Add the static method to create a basic block
Napi::Value BasicBlockWrapper::CreateBasicBlock(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 3 || !info[0].IsObject() || !info[1].IsString() || !info[2].IsObject()) {
        Napi::TypeError::New(env, "Context, name, and function arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    
    // Get the context
    auto contextWrapper = Napi::ObjectWrap<LLVMContextWrapper>::Unwrap(info[0].As<Napi::Object>());
    if (!contextWrapper) {
        Napi::TypeError::New(env, "Invalid context")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    llvm::LLVMContext& context = contextWrapper->GetContext();
    
    // Get the name
    std::string name = info[1].As<Napi::String>().Utf8Value();
    
    // Get the function
    Napi::Object funcObj = info[2].As<Napi::Object>();
    if (!funcObj.InstanceOf(FunctionWrapper::constructor.Value())) {
        Napi::TypeError::New(env, "Function argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    llvm::Function* function = Napi::ObjectWrap<FunctionWrapper>::Unwrap(funcObj)->GetFunction();
    
    // Create the basic block
    llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(context, name, function);
    
    // Return the wrapped basic block
    return BasicBlockWrapper::Create(env, basicBlock);
}

// Update the Init function for BasicBlockWrapper to include the static method
Napi::Object BasicBlockWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "BasicBlock", {
        StaticMethod("createBasicBlock", &BasicBlockWrapper::CreateBasicBlock)
        // Add other methods as needed
    });
    
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    
    exports.Set("BasicBlock", func);
    return exports;
}


PHINodeWrapper::PHINodeWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<PHINodeWrapper>(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() == 1 && info[0].IsExternal()) {
        // Unwrap the PHINode pointer passed from Create()
        phiNode_ = static_cast<llvm::PHINode*>(
            info[0].As<Napi::External<llvm::PHINode>>().Data());
    } else {
        Napi::TypeError::New(env, "PHINodeWrapper constructor is not meant to be called directly")
            .ThrowAsJavaScriptException();
    }
}

Napi::Object PHINodeWrapper::Create(Napi::Env env, llvm::PHINode* phiNode) {
    Napi::External<llvm::PHINode> external = Napi::External<llvm::PHINode>::New(env, phiNode);
    
    Napi::Object obj = constructor.New({ external });
    
    return obj;
}

Napi::Object PHINodeWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "PHINode", {
        InstanceMethod("addIncoming", &PHINodeWrapper::AddIncoming)
        // Add other methods as needed
    });
    
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    
    exports.Set("PHINode", func);
    return exports;
}

Napi::Value PHINodeWrapper::AddIncoming(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject()) {
        Napi::TypeError::New(env, "Value and BasicBlock arguments expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }

    std::cout << "Fetching value" << std::endl;
    

    llvm::Value* value = IRBuilderWrapper::UnwrapValue(info[0]);
    if (!value) {
        Napi::TypeError::New(env, "Invalid value")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
   
    
    // Get the basic block
    Napi::Object bbObj = info[1].As<Napi::Object>();
    if (!bbObj.InstanceOf(BasicBlockWrapper::constructor.Value())) {
        Napi::TypeError::New(env, "BasicBlock argument expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    llvm::BasicBlock* basicBlock = Napi::ObjectWrap<BasicBlockWrapper>::Unwrap(bbObj)->GetBasicBlock();
    
    // Add the incoming value
    phiNode_->addIncoming(value, basicBlock);
    
    return env.Undefined();
}



}  // namespace llvm_nodejs
