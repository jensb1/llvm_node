// Option 1: Using ES module syntax
import { createRequire } from 'module';
const require = createRequire(import.meta.url);

console.log('Starting test...');

// Load the LLVM Node.js addon
const llvm = require('./build/Release/llvm_nodejs');

console.log('Loaded LLVM Node.js addon');

// Create a new LLVM context
const context = new llvm.LLVMContext();
console.log('Created LLVM Context');

// Create a module with a name
const moduleName = 'my_first_module';
const module = context.createModule(moduleName);
console.log('Created module with name:', module.getName());

// Test the module name getter
console.log('Initial module name:', module.getName());

// Test the module name setter
const newName = 'renamed_module';
module.setName(newName);
console.log('After renaming, module name:', module.getName());

// Test the dump method to see the module's IR
console.log('\nModule IR dump:');
console.log(module.dump());

// Create another module to demonstrate multiple module handling
const anotherModule = context.createModule('another_module');
console.log('\nCreated another module with name:', anotherModule.getName());

// Dump the second module
console.log('\nAnother module IR dump:');
console.log(anotherModule.dump());

// Get the Int8 type from the context
const int8Type = context.getInt8Ty();
const int32Type = context.getInt32Ty();
const floatType = context.getFloatTy();
const pointerType = llvm.PointerType.get(int8Type, 0);

// Create an array type with 50 elements of Int8
const nameType = llvm.ArrayType.get(int8Type, 50);

// You can dump the type to see its representation
console.log('Array type representation:');
console.log(nameType.dump());

// Create a struct type with the array type
const structType = llvm.StructType.create(context, 'struct.Person');
structType.setBody([floatType, int8Type, nameType, pointerType]);

// You can dump the struct type to see its representation
console.log('\nStruct type representation:');
console.log(structType.dump());

// ==================== IRBuilder Demo ====================
console.log('\n========== IRBuilder Demo ==========');

// Create a function type: int32 (int32, int32)
const returnType = int32Type;
const paramTypes = [int32Type, int32Type];
const functionType = llvm.FunctionType.get(returnType, paramTypes, false);

console.log('Function type representation:', functionType.dump());

// Create a function in the module
const functionName = 'add_numbers';
const addFunction = module.createFunction(functionName, functionType);

console.log('addFunction: ', addFunction.dump());
// Create basic blocks
console.log(llvm.BasicBlock);
const entryBlock = llvm.BasicBlock.createBasicBlock(context, 'entry', addFunction);
const returnBlock = llvm.BasicBlock.createBasicBlock(context, 'return', addFunction);

console.log('entryBlock: ', entryBlock);
console.log('returnBlock: ', returnBlock);

// Create an IRBuilder
const builder = new llvm.IRBuilder(context);

console.log('builder: ', builder);

// Set the insertion point to the entry block
builder.setInsertPoint(entryBlock);

console.log("insert point set");

// Get function parameters
const param1 = addFunction.getArgument(0);
const param2 = addFunction.getArgument(1);

console.log("param1: ", param1);
console.log("param2: ", param2);

// Name the parameters
param1.setName('a');
param2.setName('b');

// Create an add instruction
console.log("create add instruction");
const sum = builder.createAdd(param1, param2, 'sum');

console.log("sum: ", sum);

// Create a branch to the return block
builder.createBr(returnBlock);

// Set insertion point to the return block
builder.setInsertPoint(returnBlock);

// Create a return instruction
builder.createRet(sum);

// Dump the function to see the generated IR
console.log('\nGenerated function IR:');
console.log(addFunction.dump());

// Create a more complex function that demonstrates control flow
console.log('\n========== Control Flow Demo ==========');

// Create a function: int32 max(int32, int32)
const maxFunctionType = llvm.FunctionType.get(int32Type, [int32Type, int32Type], false);
console.log('maxFunctionType: ', maxFunctionType.dump());
const maxFunction = module.createFunction('max', maxFunctionType);
console.log('maxFunction: ', maxFunction.dump());

// Create basic blocks
const maxEntry = maxFunction.createBasicBlock('entry');
const thenBlock = maxFunction.createBasicBlock('then');
const elseBlock = maxFunction.createBasicBlock('else');
const maxReturn = maxFunction.createBasicBlock('return');

// Set insertion point to entry block
builder.setInsertPoint(maxEntry);

// Get function parameters
const a = maxFunction.getArgument(0);
const b = maxFunction.getArgument(1);
a.setName('a');
b.setName('b');

// Compare a > b
const condition = builder.createICmpSGT(a, b, 'cmp');

// Create conditional branch
builder.createCondBr(condition, thenBlock, elseBlock);

// Then block (a is greater)
builder.setInsertPoint(thenBlock);
const resultA = a; // Use a as the result
builder.createBr(maxReturn);

// Else block (b is greater or equal)
builder.setInsertPoint(elseBlock);
const resultB = b; // Use b as the result
builder.createBr(maxReturn);

// Return block
builder.setInsertPoint(maxReturn);

// Create a PHI node to select the result based on which block we came from
const phi = builder.createPHI(int32Type, 2, 'result');

console.log("resultA: ", resultA, "thenBlock: ", thenBlock);
console.log("resultB: ", resultB, "elseBlock: ", elseBlock);
phi.addIncoming(resultA, thenBlock);
phi.addIncoming(resultB, elseBlock);

// Return the result
builder.createRet(phi);

// Dump the max function
console.log('\nGenerated max function IR:');
console.log(maxFunction.dump());

// Create a function that demonstrates memory operations
console.log('\n========== Memory Operations Demo ==========');

// Create a function: void swap(int32*, int32*)
const voidType = context.getVoidTy();
const int32PtrType = llvm.PointerType.get(int32Type, 0);
const swapFuncType = llvm.FunctionType.get(voidType, [int32PtrType, int32PtrType], false);
const swapFunction = module.createFunction('swap', swapFuncType);

// Create basic block
const swapBlock = swapFunction.createBasicBlock('entry');

// Set insertion point
builder.setInsertPoint(swapBlock);

// Get function parameters (pointers)
const ptrA = swapFunction.getArgument(0);
const ptrB = swapFunction.getArgument(1);
ptrA.setName('a_ptr');
ptrB.setName('b_ptr');

// Load values from pointers
const valA = builder.createLoad(int32Type, ptrA);
const valB = builder.createLoad(int32Type, ptrB);

// Store swapped values
builder.createStore(valA, ptrB);
builder.createStore(valB, ptrA);

// Return void
builder.createRetVoid();

// Dump the swap function
console.log('\nGenerated swap function IR:');
console.log(swapFunction.dump());

// Create a function that demonstrates struct operations
console.log('\n========== Struct Operations Demo ==========');

// Define a struct type with two integers
const intPairStruct = llvm.StructType.create(context, 'struct.IntPair');
intPairStruct.setBody([int32Type, int32Type]);

console.log('IntPair struct type:');
console.log(intPairStruct.dump());

// Create a function: int32 sumPair(struct.IntPair*)
const intPairPtrType = llvm.PointerType.get(intPairStruct, 0);
const sumPairFuncType = llvm.FunctionType.get(int32Type, [intPairPtrType], false);
const sumPairFunction = module.createFunction('sumPair', sumPairFuncType);

// Create basic block
const sumPairBlock = sumPairFunction.createBasicBlock('entry');

// Set insertion point
builder.setInsertPoint(sumPairBlock);

// Get function parameter (pointer to struct)
const pairPtr = sumPairFunction.getArgument(0);
pairPtr.setName('pair_ptr');

// Get pointers to struct fields (GEP = GetElementPtr)
const field0Ptr = builder.createStructGEP(intPairStruct, pairPtr, 0, 'field0_ptr');
const field1Ptr = builder.createStructGEP(intPairStruct, pairPtr, 1, 'field1_ptr');

// Load values from the struct fields
const field0Val = builder.createLoad(int32Type, field0Ptr);
const field1Val = builder.createLoad(int32Type, field1Ptr);

// Add the two fields
const structSum = builder.createAdd(field0Val, field1Val);

// Return the sum
builder.createRet(structSum);

// Dump the sumPair function
console.log('\nGenerated sumPair function IR:');
console.log(sumPairFunction.dump());

const verify_res = module.verify();
console.log('verify_res: ', verify_res);

// Dump the entire module to see all functions
console.log('\nFinal module IR:');
console.log(module.dump());



