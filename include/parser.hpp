#pragma once

#include "lexer.hpp"
#include <variant>
#include <optional>
#include <memory>

enum class BinOpType{
    ADD,
    SUB,
    MUL,
    DIV
};

struct IntLitNode{
    Token val;
};

struct IdentNode{
    Token val;
};

struct PrimaryExprNode{
    std::variant<IntLitNode*, IdentNode*> var;
};

struct ExprNode;

struct BinOpExpr{
    BinOpType type;
    ExprNode * lhs;
    ExprNode * rhs;
};

struct ExprNode{
    std::variant<PrimaryExprNode*, BinOpExpr*> var;
};

struct StmtNode;

struct ProtoTypeNode{
    Token name;
    Token return_type;
    int arg_counter;
    std::vector<StmtNode*> args;
};

struct FunctionNode{
    ProtoTypeNode * prototype;
    std::vector<StmtNode*> body;
};

struct LetStmtNode{
    Token type;
    Token identifier;
    ExprNode * expression;
};

struct ReturnNode{
    std::optional<std::variant<ExprNode*, IdentNode*>> retval;
};

struct StmtNode{
    std::variant<LetStmtNode*, FunctionNode*> var;
};


struct ProgNode{
    std::vector<StmtNode*> stmts;
};

class Parser{

    private:
        std::vector<Token> m_tokens;
        int m_index = 0; 

        std::optional<Token> peek(int offset);
        Token eat();
        void TryEat(TokenType token);

    public:
        Parser(std::vector<Token> tokens) : m_tokens(tokens) {}


        PrimaryExprNode * ParsePrimaryExpr();
        ExprNode * ParseFactor();
        ExprNode * ParseTerm();
        ExprNode * ParseExpr();
        ProtoTypeNode * ParseProto();
        FunctionNode * ParseFunc();
        LetStmtNode * ParseLetStmt();
        StmtNode * ParseStmt();
        ProgNode * Parse();
};
