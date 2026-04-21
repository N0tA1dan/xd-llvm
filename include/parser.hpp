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
    std::variant<std::unique_ptr<IntLitNode>, std::unique_ptr<FloatLitNode>, std::unique_ptr<IdentNode>, std::unique_ptr<ExprNode>> var;
};

struct BinOpExpr{
    BinOpType type;
    std::unique_ptr<ExprNode> lhs;
    std::unique_ptr<ExprNode> rhs;
};

struct ConditionalOpExpr{
    ConditionalOpType type;
    std::unique_ptr<ExprNode> lhs;
    std::unique_ptr<ExprNode> rhs;
};


struct ExprNode{
    std::variant<std::unique_ptr<PrimaryExprNode>, std::unique_ptr<BinOpExpr>, std::unique_ptr<ConditionalOpExpr>> var;
};

// forward decleration
struct StmtNode;

struct ProtoTypeNode{
    Token name;
    Token returnType;
    int argCounter;
    std::vector<std::unique_ptr<StmtNode>> args;
};

struct FunctionNode{
    std::unique_ptr<ProtoTypeNode> prototype;
    std::vector<std::unique_ptr<StmtNode>> body;
};

struct CompoundStmtNode{
  std::vector<std::unique_ptr<StmtNode>> body;
};

struct DeclerationStmtNode{
    Token type;
    Token identifier;
    std::optional<std::unique_ptr<ExprNode>> expression;
};

struct AssignmentNode{
    Token identifier;
    std::unique_ptr<ExprNode> expression;
};

struct IfStmtNode{
   std::unique_ptr<ExprNode> condition; 
   std::vector<std::unique_ptr<StmtNode>> thenBody;
   std::vector<std::unique_ptr<StmtNode>> elseBody;
};

struct ReturnNode{
    std::optional<std::variant<std::unique_ptr<ExprNode>, std::unique_ptr<IdentNode>>> retval;
};

struct StmtNode{
    std::variant<std::unique_ptr<FunctionNode>, std::unique_ptr<AssignmentNode>, std::unique_ptr<IfStmtNode>, std::unique_ptr<DeclerationStmtNode>, std::unique_ptr<CompoundStmtNode>> var;
};


struct ProgNode{
    std::vector<std::unique_ptr<StmtNode>> stmts;
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

        std::unique_ptr<PrimaryExprNode> ParsePrimaryExpr();
        std::unique_ptr<ExprNode> ParseFactor();
        std::unique_ptr<ExprNode> ParseTerm();
        std::unique_ptr<ExprNode> ParseExpr();
        std::unique_ptr<ExprNode> ParseComparison();
        std::unique_ptr<ExprNode> ParseEquality();
        std::unique_ptr<ProtoTypeNode> ParseProto();
        std::unique_ptr<FunctionNode> ParseFunc();
        std::unique_ptr<CompoundStmtNode> ParseCompoundStmt();
        std::unique_ptr<AssignmentNode> ParseAssignmentStmt();
        std::unique_ptr<IfStmtNode> ParseIfStmt();
        std::unique_ptr<DeclerationStmtNode> ParseDecleration();
        std::unique_ptr<StmtNode> ParseStmt();
        std::unique_ptr<ProgNode> Parse();
};
