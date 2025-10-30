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

enum class ConditionalOpType{
    EQUAL_TO,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_OR_EQUAL,
    GREATER_OR_EQUAL
};

struct IntLitNode{
    Token val;
};

struct FloatLitNode{
    Token val;
};

struct IdentNode{
    Token val;
};

struct ExprNode;

struct PrimaryExprNode{
    std::variant<IntLitNode*, FloatLitNode*, IdentNode*, ExprNode*> var;
};

struct BinOpExpr{
    BinOpType type;
    ExprNode * lhs;
    ExprNode * rhs;
};

struct ConditionalOpExpr{
    ConditionalOpType type;
    ExprNode * lhs;
    ExprNode * rhs;
};


struct ExprNode{
    std::variant<PrimaryExprNode*, BinOpExpr*, ConditionalOpExpr*> var;
};

// forward decleration
struct StmtNode;

struct ProtoTypeNode{
    Token name;
    Token returnType;
    int argCounter;
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

struct AssignmentNode{

    Token identifier;
    ExprNode* expression;
};


struct IfStmtNode{
   ExprNode * condition; 
   std::vector<StmtNode*> stmts;
};

struct ReturnNode{
    std::optional<std::variant<ExprNode*, IdentNode*>> retval;
};

struct StmtNode{
    std::variant<LetStmtNode*, FunctionNode*, AssignmentNode*, IfStmtNode*> var;
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
        ExprNode * ParseComparison();
        ExprNode * ParseEquality();
        ProtoTypeNode * ParseProto();
        FunctionNode * ParseFunc();
        LetStmtNode * ParseLetStmt();
        AssignmentNode * ParseAssignmentStmt();
        IfStmtNode * ParseIfStmt();
        StmtNode * ParseStmt();
        ProgNode * Parse();
};
