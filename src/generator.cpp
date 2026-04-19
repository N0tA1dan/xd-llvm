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


std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> TheModule;

std::map<std::string, llvm::Value *> GlobalValues;
std::map<std::string, VarInfo> NamedValues;
llvm::Function * CurrentFunc = nullptr;

static llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction, llvm::Type * Type, llvm::StringRef VarName) {
    if (!TheFunction) return nullptr;
    llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(Type, nullptr, VarName);
}

void InitializeModule(){
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("XD Compiler", *TheContext);
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

llvm::Type* Generator::GetTypeFromToken(TokenType type) {
    switch(type){
        case TokenType::INT:
            return llvm::Type::getInt32Ty(*TheContext);
        case TokenType::UINT:
            return llvm::Type::getInt32Ty(*TheContext);
        case TokenType::FLOAT:
            return llvm::Type::getFloatTy(*TheContext);
        case TokenType::VOID:
            return llvm::Type::getVoidTy(*TheContext);
        default:
            llvm::errs() << "DEBUG: Unhandled TokenType (" << (int)type << ") in GetTypeFromToken.\n";
            return nullptr;
    }
}

TypedValue Generator::GenPrimaryExpr(const std::unique_ptr<PrimaryExprNode>& primaryExpr){
    struct PrimaryExprVisitor{
        Generator & generator;
        TypedValue value = {nullptr, false};

        void operator()(const std::unique_ptr<IntLitNode>& intLit){
            std::string intValueStr = intLit->val.value.value();
            value.value = Builder->getInt32(std::stoi(intValueStr));
        }

        void operator()(const std::unique_ptr<FloatLitNode>& floatLit){
            std::string floatValueStr = floatLit->val.value.value();
            value.value = llvm::ConstantFP::get(llvm::Type::getFloatTy(*TheContext), std::stof(floatValueStr));
        }

        void operator()(const std::unique_ptr<IdentNode>& ident){
            if(CurrentFunc == nullptr){
                return;
            }

            if(CurrentFunc != nullptr){
                std::string variableName = ident->val.value.value();

                if(NamedValues.find(variableName) == NamedValues.end()){
                    llvm::errs() << "Error: Undefined variable: " << variableName << '\n';
                    value = {nullptr, false};
                    return;
                }

                VarInfo info = NamedValues.at(variableName);
                value.isUnsigned = info.isUnsigned;
                llvm::AllocaInst* variable = info.alloca;
                llvm::Type* variableType = variable->getAllocatedType();
                value.value = Builder->CreateLoad(variableType, variable);
            } 
        }
        
        void operator()(const std::unique_ptr<ExprNode>& innerExpr){
            value = generator.GenExpr(innerExpr);
        }
    };

    PrimaryExprVisitor visitor = {*this};
    std::visit(visitor, primaryExpr->var);
    return visitor.value;
}

TypedValue Generator::GenExpr(const std::unique_ptr<ExprNode>& expr){

    if (!expr) {
        llvm::errs() << "ERROR: GenExpr called with nullptr ExprNode\n";
        return {nullptr, false};
    }

    struct ExprVisitor{
        Generator & generator;
        TypedValue value = {nullptr, false};

        void operator()(const std::unique_ptr<PrimaryExprNode>& primaryExpr){
            value = generator.GenPrimaryExpr(primaryExpr);
        }

        void operator()(const std::unique_ptr<BinOpExpr>& binExpr){

            TypedValue lhs = generator.GenExpr(binExpr->lhs);
            TypedValue rhs = generator.GenExpr(binExpr->rhs);

            if (!lhs.value || !rhs.value) {
                llvm::errs() << "ERROR: Null operand encountered in binary expression.\n";
                value = {nullptr, false};
                return;
            }

            llvm::Type * leftType = lhs.value->getType();
            llvm::Type * rightType = rhs.value->getType();

            if(leftType->isIntegerTy() && rightType->isIntegerTy()){
                bool isUnsigned = lhs.isUnsigned || rhs.isUnsigned;

                switch(binExpr->type){
                    case BinOpType::ADD:
                        value = {Builder->CreateAdd(lhs.value, rhs.value), isUnsigned};
                        break;
                    case BinOpType::SUB:
                        value = {Builder->CreateSub(lhs.value, rhs.value), isUnsigned};
                        break;
                    case BinOpType::MUL:
                        value = {Builder->CreateMul(lhs.value, rhs.value), isUnsigned};
                        break;
                    case BinOpType::DIV:
                        value = isUnsigned
                            ? TypedValue{Builder->CreateUDiv(lhs.value, rhs.value), true}
                            : TypedValue{Builder->CreateSDiv(lhs.value, rhs.value), false};
                        break;
                    default:
                        llvm::errs() << "DEBUG: Error unknown operation between expression\n"; 
                        break;
                }
                return;
            }

            if(leftType->isFloatingPointTy() && rightType->isFloatingPointTy()){
                switch(binExpr->type){
                    case BinOpType::ADD:
                        value.value = Builder->CreateFAdd(lhs.value, rhs.value);
                        break;
                    case BinOpType::SUB:
                        value.value = Builder->CreateFSub(lhs.value, rhs.value);
                        break;
                    case BinOpType::MUL:
                        value.value = Builder->CreateFMul(lhs.value, rhs.value);
                        break;
                    case BinOpType::DIV:
                        value.value = Builder->CreateFDiv(lhs.value, rhs.value);
                        break;
                    default:
                        llvm::errs() << "DEBUG: Error unknown operation between expression\n"; 
                        break;
                }
                return;
            }

            llvm::errs() << "ERROR: Invalid operand types for binary expression. LHS="
                 << *leftType << " RHS=" << *rightType << "\n";
            value = {nullptr, false};
        }

        void operator()(const std::unique_ptr<ConditionalOpExpr>& conditionalExpr){
            TypedValue lhs = generator.GenExpr(conditionalExpr->lhs);
            TypedValue rhs = generator.GenExpr(conditionalExpr->rhs);

            llvm::Type * leftType = lhs.value->getType();
            llvm::Type * rightType = rhs.value->getType();

            if(leftType->isIntegerTy() && rightType->isIntegerTy()){
                bool isUnsigned = lhs.isUnsigned || rhs.isUnsigned;

                switch(conditionalExpr->type){
                    case ConditionalOpType::EQUAL_TO:
                        value.value = Builder->CreateICmpEQ(lhs.value, rhs.value);
                        break;
                    case ConditionalOpType::NOT_EQUAL:
                        value.value = Builder->CreateICmpNE(lhs.value, rhs.value);
                        break;
                    case ConditionalOpType::LESS_THAN:
                        value.value = isUnsigned
                            ? Builder->CreateICmpULT(lhs.value, rhs.value)
                            : Builder->CreateICmpSLT(lhs.value, rhs.value);
                        break;
                }
            }

            else if(leftType->isFloatingPointTy() && rightType->isFloatingPointTy()){
                switch(conditionalExpr->type){
                    case ConditionalOpType::EQUAL_TO:
                        value.value = Builder->CreateFCmpOEQ(lhs.value, rhs.value);
                        break;
                    case ConditionalOpType::NOT_EQUAL:
                        value.value = Builder->CreateFCmpONE(lhs.value, rhs.value);
                        break;
                    case ConditionalOpType::LESS_THAN:
                        value.value = Builder->CreateFCmpOLT(lhs.value, rhs.value);
                        break;
                }
            }

            else{
                llvm::errs() << "Error Comparison between expressions failed, type mismatch\n";
            }
        }
    };

    ExprVisitor visitor = {*this};
    std::visit(visitor, expr->var);
    return visitor.value;
}

void Generator::GenStmt(const std::unique_ptr<StmtNode>& stmt){
    struct StmtVisitor{
        Generator & generator;
        
        void operator()(const std::unique_ptr<DeclerationStmtNode>& decleration){
            llvm::Type * VarType = generator.GetTypeFromToken(decleration->type.type);
            if (!VarType) return;

            if (CurrentFunc == nullptr) {
                llvm::Constant * Initializer = nullptr;
                if (VarType->isIntegerTy()) {
                    Initializer = llvm::ConstantInt::get(VarType, 0);
                } else if (VarType->isFloatTy()) {
                    Initializer = llvm::ConstantFP::get(VarType, 0.0);
                } else {
                    return;
                }

                llvm::GlobalVariable* GlobalVar = new llvm::GlobalVariable(
                    *TheModule, VarType, false,
                    llvm::GlobalValue::ExternalLinkage,
                    Initializer,
                    decleration->identifier.value.value()
                );
                
                GlobalValues[decleration->identifier.value.value()] = GlobalVar;
                return;

            } else {
                llvm::AllocaInst* Alloc = CreateEntryBlockAlloca(CurrentFunc, VarType, decleration->identifier.value.value());
                
                if (VarType->isIntegerTy(32) || VarType->isFloatTy()) {
                    if(decleration->expression.has_value()){
                        TypedValue InitialValue = generator.GenExpr(decleration->expression.value());
                        if (InitialValue.value) {
                            Builder->CreateStore(InitialValue.value, Alloc);
                        } else {
                            llvm::errs() << "ERROR: Failed to generate IR for initializer expression of variable: " << decleration->identifier.value.value() << "\n";
                        }
                    }
                }
                
                VarInfo info;
                info.isUnsigned = (decleration->type.type == TokenType::UINT);
                info.alloca = Alloc;
                NamedValues[decleration->identifier.value.value()] = info;
                return;
            }
        }

        void operator()(const std::unique_ptr<FunctionNode>& Function){

            llvm::Type* ReturnType = generator.GetTypeFromToken(Function->prototype->returnType.type);
            
            if (!ReturnType) {
                llvm::errs() << "DEBUG: Function Gen failed - ReturnType is null for function: " 
                             << Function->prototype->name.value.value() << "\n";
                return;
            } 

            llvm::FunctionType * funcType = nullptr;

            if(Function->prototype->argCounter == 0){
                funcType = llvm::FunctionType::get(ReturnType, false);
            } else{
                funcType = llvm::FunctionType::get(ReturnType, false);
            }

            llvm::Function* func = llvm::Function::Create(
                funcType,
                llvm::Function::ExternalLinkage,
                Function->prototype->name.value.value().c_str(),
                TheModule.get()
            );

            llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*TheContext, "entry", func);
            Builder->SetInsertPoint(entryBB);

            NamedValues.clear();
            CurrentFunc = func;

            for(const auto& stmt : Function->body){
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
            CurrentFunc = nullptr;
        }

        void operator()(const std::unique_ptr<AssignmentNode>& assignment){
            if(CurrentFunc != nullptr){
                if(NamedValues.find(assignment->identifier.value.value()) == NamedValues.end()){
                    llvm::errs() << "ERROR: Variable is uninitialized\n";
                    exit(EXIT_FAILURE);
                }
                
                TypedValue newValue = generator.GenExpr(assignment->expression);
                VarInfo& info = NamedValues.at(assignment->identifier.value.value());

                if(newValue.value){
                    Builder->CreateStore(newValue.value, info.alloca);
                } else {
                    llvm::errs() << "ERROR: Unexpected error generating expression for assignment operation\n";
                }
            }
        }

        void operator()(const std::unique_ptr<IfStmtNode>& ifStmt){
            if(CurrentFunc == nullptr){
                llvm::errs() << "ERROR: If statement must be contained within a function\n";
                exit(EXIT_FAILURE);
            }

            TypedValue condition = generator.GenExpr(ifStmt->condition);

            llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*TheContext, "then", CurrentFunc);
            llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*TheContext, "else", CurrentFunc);
            llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*TheContext, "merge", CurrentFunc);

            Builder->CreateCondBr(condition.value, thenBB, elseBB);
            Builder->SetInsertPoint(thenBB);

            for (const auto& stmt : ifStmt->thenBody) {
                generator.GenStmt(stmt);
            }
            if (!Builder->GetInsertBlock()->getTerminator()) {
                Builder->CreateBr(mergeBB);
            }

            Builder->SetInsertPoint(elseBB);
            if (!ifStmt->elseBody.empty()) {
                for (const auto& stmt : ifStmt->elseBody) {
                    generator.GenStmt(stmt);
                }
            }
            if (!Builder->GetInsertBlock()->getTerminator()) {
                Builder->CreateBr(mergeBB);
            }

            Builder->SetInsertPoint(mergeBB);
        }
    };

    std::visit(StmtVisitor{*this}, stmt->var);
}

void Generator::Generate(const std::unique_ptr<ProgNode>& prog){
    InitializeModule();

    for(const auto& stmt : prog->stmts){
        Generator::GenStmt(stmt);
    }

    TheModule->print(llvm::errs(), nullptr);
}
