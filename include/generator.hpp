#pragma once

#include "parser.hpp"
#include <iostream>
#include <map>
#include "lexer.hpp"
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


class Generator{
    private:
        std::unique_ptr<ProgNode> m_prog;

    public:
        Generator(std::unique_ptr<ProgNode> prog) : m_prog(std::move(prog)) {}

        // --- ADDED/MODIFIED DECLARATIONS BELOW ---
        // New helper function to get LLVM Type
        llvm::Type* GetTypeFromToken(TokenType type); 
        
        // Return type changed from void to llvm::Value*
        llvm::Value* GenPrimaryExpr(const std::unique_ptr<PrimaryExprNode>& primaryExpr);
        
        // Return type changed from void to llvm::Value*
        llvm::Value* GenExpr(const std::unique_ptr<ExprNode>& expr);

        
        void GenStmt(const std::unique_ptr<StmtNode>& stmt);
        void Generate();

};