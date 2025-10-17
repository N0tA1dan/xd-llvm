#include "generator.hpp"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
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

// holds current variable type globally
llvm::Type * VarType = nullptr;
// for doing alloc stuff in functions. like %stack = alloca i32
// had to modify to accept llvm::Type because the kaleidoscope tutorial was just treating everything as a double
static llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction, llvm::Type * Type,llvm::StringRef VarName) {
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

void Generator::GenPrimaryExpr(PrimaryExprNode * primaryExpr){
    struct PrimaryExprVisitor{

        void operator()(IntLitNode * intLit){

        }

        void operator()(IdentNode * ident){

        }
        
        void operator()(ExprNode * innerExpr){

        }
    };

    std::visit(PrimaryExprVisitor{}, primaryExpr->var);
}

void Generator::GenExpr(ExprNode * expr){
    struct ExprVisitor{
        Generator & generator;

        void operator()(PrimaryExprNode * primaryExpr){
        }

        void operator()(BinOpExpr * binExpr){

        }


    };
    std::visit(ExprVisitor{*this}, expr->var);
}

void Generator::GenStmt(StmtNode * stmt){
    struct StmtVisitor{
        Generator & generator;
        

        void operator()(LetStmtNode * LetStmt){
            llvm::Type * VarType = nullptr;

            switch(LetStmt->type.type){
                case TokenType::INT:
                    VarType = llvm::Type::getInt32Ty(*TheContext);
                    break;
                case TokenType::FLOAT:
                    VarType = llvm::Type::getFloatTy(*TheContext);
                    break;
                case TokenType::VOID:
                    VarType = llvm::Type::getVoidTy(*TheContext);
                    break;
            }

            llvm::AllocaInst* Alloc = CreateEntryBlockAlloca(CurrentFunc, VarType, LetStmt->identifier.value.value());
            // fix this to instead call gen expression
            Builder->CreateStore(Builder->getInt32(10), Alloc);
            NamedValues[LetStmt->identifier.value.value()] = Alloc;
            
            return;
        }

        void operator()(FunctionNode * Function){

            llvm::Type* ReturnType = nullptr; 
            // should prob make a function to do this 
            switch(Function->prototype->returnType.type){
                case TokenType::INT:
                    ReturnType = llvm::Type::getInt32Ty(*TheContext);
                    break;
                case TokenType::FLOAT:
                    ReturnType = llvm::Type::getFloatTy(*TheContext);
                    break;
                case TokenType::VOID:
                    ReturnType = llvm::Type::getVoidTy(*TheContext);
                    break;
            }

            // initializes the function object and checks if function has any arguments/parameters
            llvm::FunctionType * funcType = nullptr;

            if(Function->prototype->argCounter== 0){
                funcType = llvm::FunctionType::get(ReturnType, false);
            } else{
                // make sure to create a func with arguments here
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
        }
    };


    std::visit(StmtVisitor{*this}, stmt->var);
}

void Generator::Generate(){
    // intializes llvm's stuff to generate stuff 
    InitializeModule();
    for(auto stmt : m_prog->stmts){
        Generator::GenStmt(stmt);
    }

    TheModule->print(llvm::errs(), nullptr);

}
