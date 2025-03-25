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
        ProgNode * m_prog;

    public:
        Generator(ProgNode * prog);
        
        void GenPrimaryExpr(PrimaryExprNode * primaryExpr);
        void GenExpr(ExprNode * expr);
        void GenStmt(StmtNode * stmt);
        void Generate();

};
