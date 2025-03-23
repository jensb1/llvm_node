#include <napi.h>
#include "llvm_context.h"
#include "llvm_function.h"
#include "llvm_builder.h"
#include "llvm_module.h"
#include "llvm_types.h"
#include "llvm_builder.h"
#include <iostream>
namespace llvm_nodejs {

extern Napi::Object InitContext(Napi::Env env, Napi::Object exports);
extern Napi::Object InitModule(Napi::Env env, Napi::Object exports);
extern Napi::Object InitTypes(Napi::Env env, Napi::Object exports);
extern Napi::Object InitIRBuilder(Napi::Env env, Napi::Object exports);

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    std::cout << "Initializing LLVM Node.js addon" << std::endl;
    exports = LLVMContextWrapper::Init(env, exports);
    std::cout << "Initialized LLVM Context" << std::endl;
    exports = ModuleWrapper::Init(env, exports);
    std::cout << "Initialized LLVM Module" << std::endl;
    
    // Initialize ArgumentWrapper before it's used in InitValueWrappers
    exports = ArgumentWrapper::Init(env, exports);
    std::cout << "Initialized LLVM Argument" << std::endl;
    exports = IRBuilderWrapper::Init(env, exports);
    std::cout << "Initialized LLVM IR Builder" << std::endl;
    exports = InitTypes(env, exports);
    std::cout << "Initialized LLVM Types" << std::endl;
    exports = FunctionWrapper::Init(env, exports);
    std::cout << "Initialized LLVM Function" << std::endl;
    exports = ValueWrapper::Init(env, exports);
    std::cout << "Initialized LLVM Value" << std::endl;
    exports = ConstantWrapper::Init(env, exports);
    std::cout << "Initialized LLVM Constant" << std::endl;
    exports = InstructionWrapper::Init(env, exports);
    std::cout << "Initialized LLVM Instruction" << std::endl;
    exports = BasicBlockWrapper::Init(env, exports);
    std::cout << "Initialized LLVM Basic Block" << std::endl;
    exports = PHINodeWrapper::Init(env, exports);
    std::cout << "Initialized LLVM PHINode" << std::endl;

    return exports;
}

NODE_API_MODULE(llvm_nodejs, InitAll)

}  // namespace llvm_nodejs
