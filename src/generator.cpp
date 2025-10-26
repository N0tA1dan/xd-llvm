#include "generator.hpp"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h" 
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>


Generator::Generator(ProgNode * prog) : m_prog(prog){}


std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> TheModule;

// stores global variables declared outside functions.
std::map<std::string, llvm::Value *> GlobalValues;

// stores local variables inside functions. Gets cleared after every generation of a function
std::map<std::string, llvm::AllocaInst*> NamedValues;

// holds the current function pointer globally to know where to insert variables and other stuff. 
llvm::Function * CurrentFunc = nullptr;

// for doing alloc stuff in functions. like %stack = alloca i32
static llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction, llvm::Type * Type,llvm::StringRef VarName) {
    // Check if the function is valid before accessing its members
    if (!TheFunction) return nullptr;
    
    llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(Type, nullptr,VarName);
}

void InitializeModule(){
    // Open a new context and module.
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("XD Compiler", *TheContext);

    // Create a new builder for the module.
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

llvm::Type* Generator::GetTypeFromToken(TokenType type) {
    switch(type){
        case TokenType::INT:
            return llvm::Type::getInt32Ty(*TheContext);
        case TokenType::FLOAT:
            return llvm::Type::getFloatTy(*TheContext);
        case TokenType::VOID:
            return llvm::Type::getVoidTy(*TheContext);
        default:
            // DEBUG: Log the unhandled token type value
            llvm::errs() << "DEBUG: Unhandled TokenType (" << (int)type << ") in GetTypeFromToken.\n";
            return nullptr;
    }
}

llvm::Value* Generator::GenPrimaryExpr(PrimaryExprNode * primaryExpr){
    struct PrimaryExprVisitor{
        Generator & generator;
        llvm::Value* result = nullptr; // Holds the result of the visit

        void operator()(IntLitNode * intLit){
            
            std::string intValueStr = intLit->val.value.value();
            
            result = Builder->getInt32(std::stoi(intValueStr));
        }

        void operator()(FloatLitNode* floatLit){
            
            std::string floatValueStr = floatLit->val.value.value();
            
            result = llvm::ConstantFP::get(llvm::Type::getFloatTy(*TheContext), std::stof(floatValueStr));
        }

        void operator()(IdentNode * ident){

            // global variable generation
            if(CurrentFunc == nullptr){
                
            }

            // currently inside a function
            if(CurrentFunc != nullptr){

                std::string variableName = ident->val.value.value();

                if(NamedValues.find(ident->val.value.value()) == NamedValues.end()){
                    llvm::errs() << "Error: Undefined variable: " << variableName << '\n';
                    result = nullptr;
                }

                llvm::AllocaInst* variable = NamedValues.at(variableName);
                llvm::Type* variableType = variable->getAllocatedType();
                result = Builder->CreateLoad(variableType, variable);
            } 
        }
        
        // Handles expressions within parenthesis. Eg: (10*5)
        void operator()(ExprNode * innerExpr){
            result = generator.GenExpr(innerExpr);
        }
    };

    PrimaryExprVisitor visitor = {*this};
    std::visit(visitor, primaryExpr->var);
    return visitor.result;
}

llvm::Value* Generator::GenExpr(ExprNode * expr){

    if (!expr) {
        llvm::errs() << "ERROR: GenExpr called with nullptr ExprNode\n";
        return nullptr;
    }


    struct ExprVisitor{
        Generator & generator;
        llvm::Value* result = nullptr; 

        void operator()(PrimaryExprNode * primaryExpr){
            result = generator.GenPrimaryExpr(primaryExpr);
        }

        void operator()(BinOpExpr * binExpr){

            llvm::Value * leftHandSide = generator.GenExpr(binExpr->lhs);
            llvm::Value * rightHandSide = generator.GenExpr(binExpr->rhs);

            llvm::Type * leftType = leftHandSide->getType();
            llvm::Type * rightType = rightHandSide->getType();
            
            // Check for null operands (like from the unhandled PrimaryExpr case)
            if (!leftHandSide || !rightHandSide) {
                llvm::errs() << "ERROR: Null operand encountered in binary expression.\n";
                result = nullptr;
                return;
            }

            // creates operations specifically for integer types
            if(leftType->isIntegerTy() == true && rightType->isIntegerTy() == true){
                switch(binExpr->type){
                    case BinOpType::ADD:
                        result = Builder->CreateAdd(leftHandSide, rightHandSide);
                        break;

                    case BinOpType::SUB:
                        result = Builder->CreateSub(leftHandSide, rightHandSide);
                        break;
                    case BinOpType::MUL:
                        result = Builder->CreateMul(leftHandSide, rightHandSide);
                        break;
                    default:
                        llvm::errs() << "DEBUG: Error unknown operation between expression" << '\n'; 
                        break;
                }
            }

            // use floating point llvm instructions instead
            if(leftType->isFloatingPointTy() == true && rightType->isFloatingPointTy() == true){

                switch(binExpr->type){
                    case BinOpType::ADD:
                        result = Builder->CreateFAdd(leftHandSide, rightHandSide);
                        break;

                    case BinOpType::SUB:
                        result = Builder->CreateFSub(leftHandSide, rightHandSide);
                        break;
                    case BinOpType::MUL:
                        result = Builder->CreateFMul(leftHandSide, rightHandSide);
                        break;
                    default:
                        llvm::errs() << "DEBUG: Error unknown operation between expression" << '\n'; 
                        break;
                }
            }

            else {
                llvm::errs() << "Erorr: Invalid operand type for expression \n";
            }
        }
    };

    ExprVisitor visitor = {*this};
    std::visit(visitor, expr->var);
    return visitor.result;
}

void Generator::GenStmt(StmtNode * stmt){
    struct StmtVisitor{
        Generator & generator;
        
        void operator()(LetStmtNode * LetStmt){
            // Fixed the call: The helper is a member of Generator, not the StmtVisitor.
            llvm::Type * VarType = generator.GetTypeFromToken(LetStmt->type.type);
            if (!VarType) return; // Error handling for unknown type

            // Handle Global vs Local Declarations
            if (CurrentFunc == nullptr) {
                // Global Variable Declaration
                
                llvm::Constant * Initializer = nullptr;
                if (VarType->isIntegerTy()) {
                    // Default to 0 for integers
                    Initializer = llvm::ConstantInt::get(VarType, 0);
                } else if (VarType->isFloatTy()) {
                    // Default to 0.0 for floats
                    Initializer = llvm::ConstantFP::get(VarType, 0.0);
                } else {
                    // Handle other types or skip
                    return;
                }

                llvm::GlobalVariable* GlobalVar = new llvm::GlobalVariable(
                    *TheModule, // Module to add to
                    VarType,    // Type
                    false,      // isConstant (Mutable)
                    llvm::GlobalValue::ExternalLinkage, // Linkage type
                    Initializer, // Initializer
                    LetStmt->identifier.value.value() // Name
                );
                
                GlobalValues[LetStmt->identifier.value.value()] = GlobalVar;
                return;

            } else {
                // Local Variable Declaration (inside a function)
                llvm::AllocaInst* Alloc = CreateEntryBlockAlloca(CurrentFunc, VarType, LetStmt->identifier.value.value());
                
                if (VarType->isIntegerTy(32) || VarType->isFloatTy()) {
                    // This section now needs to call GenExpr
                    llvm::Value* InitialValue = generator.GenExpr(LetStmt->expression);
                    
                    if (InitialValue) {
                         Builder->CreateStore(InitialValue, Alloc);
                    } else {
                        // Handle case where InitialValue is null (e.g., if there was an error in GenExpr)
                        llvm::errs() << "ERROR: Failed to generate IR for initializer expression of variable: " << LetStmt->identifier.value.value() << "\n";
                    }
                }
                
                NamedValues[LetStmt->identifier.value.value()] = Alloc;
                return;
            }
        }

        void operator()(FunctionNode * Function){

            llvm::Type* ReturnType = generator.GetTypeFromToken(Function->prototype->returnType.type);
            
            if (!ReturnType) {
                // DEBUG: Log the failure and function name
                llvm::errs() << "DEBUG: Function Gen failed - ReturnType is null for function: " 
                             << Function->prototype->name.value.value() << "\n";
                return; // Early exit if return type is unknown/unhandled
            } 

            // initializes the function object and checks if function has any arguments/parameters
            llvm::FunctionType * funcType = nullptr;

            if(Function->prototype->argCounter== 0){
                funcType = llvm::FunctionType::get(ReturnType, false);
            } else{
                // make sure to create a func with arguments here
                // Placeholder for argument handling
                std::vector<llvm::Type*> ArgTypes;
                // Assuming Function->prototype->args is a vector of Type/Ident pairs
                // for (const auto& arg : Function->prototype->args) {
                //     ArgTypes.push_back(generator.GetTypeFromToken(arg.type.type));
                // }
                // funcType = llvm::FunctionType::get(ReturnType, ArgTypes, false);
                
                // Using no args for now until AST structure for args is clearer
                funcType = llvm::FunctionType::get(ReturnType, false);
            }

            // Create function object
            llvm::Function* func = llvm::Function::Create(
                funcType,
                llvm::Function::ExternalLinkage,
                Function->prototype->name.value.value().c_str(),
                TheModule.get()
            );

            // Create entry block
            llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*TheContext, "entry", func);
            Builder->SetInsertPoint(entryBB);

            // clear all previously declared local variables and set the global function variable to our new function
            NamedValues.clear();
            CurrentFunc = func;


            // generate all statements made in function body
            for(auto stmt : Function->body){
                generator.GenStmt(stmt);
            }

            if (!Builder->GetInsertBlock()->getTerminator()) {
                if (ReturnType->isVoidTy()) {
                    Builder->CreateRetVoid();
                } else {
                    Builder->CreateRet(llvm::Constant::getNullValue(ReturnType));
                }
            }

            llvm::verifyFunction(*func);
            // Clear CurrentFunc after function generation completes
            CurrentFunc = nullptr;
        }

        void operator()(AssignmentNode * assignment){
            
            // inside a function
            if(CurrentFunc != nullptr){
                
                
                if(NamedValues.find(assignment->identifier.value.value()) == NamedValues.end()){
                    llvm::errs() << "ERROR: Variable is unitialized" << '\n';
                    exit(EXIT_FAILURE);
                }
                
                llvm::Value * newValue = generator.GenExpr(assignment->expression);
                llvm::Value * ptrToVariable = NamedValues.at(assignment->identifier.value.value());

                if(newValue){
                    Builder->CreateStore(newValue, ptrToVariable);
                } else {
                    llvm::errs() << "ERROR: Unexpected error generating expression for assignment operation" << '\n';
                }

            }

            // within global scope
            else if(CurrentFunc == nullptr){

            }
        }

    };


    std::visit(StmtVisitor{*this}, stmt->var);
}

void Generator::Generate(){

    // initialize llvm and get ready to begin codegen
    InitializeModule();

    for(auto stmt : m_prog->stmts){
        Generator::GenStmt(stmt);
    }

    TheModule->print(llvm::errs(), nullptr);

}
