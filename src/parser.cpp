#include "parser.hpp"

std::optional<Token> Parser::peek(int offset = 0){
    if(offset + m_index >= m_tokens.size()){
        return std::nullopt;
    } else{
        return m_tokens.at(offset + m_index);
    }
}

Token Parser::eat(){
    return m_tokens.at(m_index++);
}


PrimaryExprNode * Parser::ParsePrimaryExpr(){
    PrimaryExprNode * primaryexpr = new PrimaryExprNode;

    if(peek().has_value()){

        switch(peek().value().type){
            case TokenType::INT_LIT:
                {
                    IntLitNode * intlit = new IntLitNode;
                    intlit->val = eat();
                    primaryexpr->var = intlit;
                    break;
                }

            case TokenType::IDENT:
                {
                    IdentNode * ident = new IdentNode;
                    ident->val = eat();
                    primaryexpr->var = ident;
                    break;
                }

            case TokenType::OPEN_PAREN:
                {
                    eat();

                    ExprNode * innerExpr = ParseExpr();

                    TryEat(TokenType::CLOSE_PAREN);

                    primaryexpr->var = innerExpr;

                    break;
                }

            default:
                std::cerr << "Unexpected token type found while parsing primary expression" << std::endl;
                break;
        }
    }
    return primaryexpr;
}

ExprNode * Parser::ParseFactor(){
    auto lhs = ParsePrimaryExpr();

    if(!lhs){
        std::cerr << "Error parsing primary expression in parse factor" << std::endl;
        exit(EXIT_FAILURE);
    } 

    ExprNode * lhsexpr = new ExprNode;
    lhsexpr->var = lhs;

    bool isBinExpr = false;
    
    while(peek().has_value()){

        switch(peek().value().type){

            // left off here
            case TokenType::MUL:
                {
                    isBinExpr = true;
                    eat(); // eats * token
                    auto rhs = ParsePrimaryExpr();
                    ExprNode * rhsexpr = new ExprNode;
                    rhsexpr->var = rhs;

                    BinOpExpr * binexpr = new BinOpExpr; 
                    binexpr->type = BinOpType::MUL;

                    binexpr->lhs = lhsexpr;
                    binexpr->rhs = rhsexpr;

                    ExprNode * newexpr = new ExprNode;
                    newexpr->var = binexpr;
                    lhsexpr = newexpr;

                    break;
                }

            case TokenType::DIV:
                {
                    isBinExpr = true;
                    eat(); // eats * token
                    auto rhs = ParsePrimaryExpr();
                    ExprNode * rhsexpr = new ExprNode;
                    rhsexpr->var = rhs;

                    BinOpExpr * binexpr = new BinOpExpr; 
                    binexpr->type = BinOpType::DIV;

                    binexpr->lhs = lhsexpr;
                    binexpr->rhs = rhsexpr;

                    ExprNode * newexpr = new ExprNode;
                    newexpr->var = binexpr;
                    lhsexpr = newexpr;

                    break;
                }

            default:
                {
                    isBinExpr = false;
                    break;
                }
        }

        if(isBinExpr == false) break;
    }

    return lhsexpr;

}

ExprNode * Parser::ParseTerm(){

    auto lhs = ParseFactor();

    if(!lhs){
        std::cerr << "Error parsing primary expression in parse factor" << std::endl;
        exit(EXIT_FAILURE);
    } 

    ExprNode * lhsexpr = new ExprNode;
    lhsexpr = lhs;

    bool isBinExpr = false;
    
    while(peek().has_value()){

        switch(peek().value().type){

            // left off here
            case TokenType::ADD:
                {
                    isBinExpr = true;
                    eat(); // eats * token
                    auto rhs = ParseFactor();
                    ExprNode * rhsexpr = new ExprNode;
                    rhsexpr = rhs;

                    BinOpExpr * binexpr = new BinOpExpr; 
                    binexpr->type = BinOpType::ADD;

                    binexpr->lhs = lhsexpr;
                    binexpr->rhs = rhsexpr;

                    ExprNode * newexpr = new ExprNode;
                    newexpr->var = binexpr;
                    lhsexpr = newexpr;

                    break;
                }

            case TokenType::SUB:
                {
                    isBinExpr = true;
                    eat(); // eats * token
                    auto rhs = ParseFactor();
                    ExprNode * rhsexpr = new ExprNode;
                    rhsexpr = rhs;

                    BinOpExpr * binexpr = new BinOpExpr; 
                    binexpr->type = BinOpType::SUB;

                    binexpr->lhs = lhsexpr;
                    binexpr->rhs = rhsexpr;

                    ExprNode * newexpr = new ExprNode;
                    newexpr->var = binexpr;
                    lhsexpr = newexpr;

                    break;
                }

            default:
                {
                    isBinExpr = false;
                    break;
                }
        }

        if(isBinExpr == false) break;
    }

    return lhsexpr;
}

ExprNode * Parser::ParseExpr(){
    ExprNode * expr = ParseTerm();
    return expr;

}

void Parser::TryEat(TokenType token){
    if(peek().has_value() && peek().value().type == token){
        eat();
    } else{
        switch(token){
            case TokenType::OPEN_PAREN:
                std::cerr << "Error, expected '('" << std::endl;
                break;
            case TokenType::CLOSE_PAREN:
                std::cerr << "Error, expected ')'" << std::endl;
                break;
            case TokenType::OPEN_BRACKET:
                std::cerr << "Error, expected '{'" << std::endl;
                break;
            case TokenType::CLOSE_BRACKET:
                std::cerr << "Error, expected '}'" << std::endl;
                break;
        }
        exit(EXIT_FAILURE);
    }
}

ProtoTypeNode * Parser::ParseProto(){
    auto proto = new ProtoTypeNode;
    proto->returnType= eat(); // eat return type
    proto->name = eat(); // eat name

    TryEat(TokenType::OPEN_PAREN);
 
    proto->argCounter= 0;
    // try to parse args?
    while(peek().has_value() && peek().value().type != TokenType::CLOSE_PAREN){
        proto->args.push_back(ParseStmt());
        proto->argCounter++;
    }
    TryEat(TokenType::CLOSE_PAREN);
    return proto;
}

FunctionNode * Parser::ParseFunc(){
    auto prototype = ParseProto(); 
    if(!prototype){
        std::cerr<<"error parsing prototype for function" << std::endl;
        exit(EXIT_FAILURE);
    }
    FunctionNode * func = new FunctionNode;
    func->prototype = prototype;

    TryEat(TokenType::OPEN_BRACKET);

    while(peek().has_value() && peek().value().type != TokenType::CLOSE_BRACKET){
        func->body.push_back(ParseStmt());
    }
    TryEat(TokenType::CLOSE_BRACKET);
    return func;
}

LetStmtNode * Parser::ParseLetStmt(){
    LetStmtNode * letstmt = new LetStmtNode;

    letstmt->type = eat();

    letstmt->identifier = eat();

    TryEat(TokenType::EQUALS);

    letstmt->expression = ParseExpr();

    TryEat(TokenType::SEMI);

    return letstmt;

}

StmtNode * Parser::ParseStmt(){
    auto stmt = new StmtNode;
    // handles functions
    if(peek().value().type == TokenType::FN){
        eat(); // eat fn token
        FunctionNode * func = ParseFunc();
        if(!func){
            std::cerr << "error parsing function" << std::endl;
            exit(EXIT_FAILURE);
        }
        stmt->var = func;
        
    }

    else if(peek().value().type == TokenType::LET){
        eat(); // eats let token
        LetStmtNode * letstmt = ParseLetStmt();

        if(!letstmt){
            std::cerr << "error parsing let statement" << std::endl;
            exit(EXIT_FAILURE);
        }
        stmt->var = letstmt;
    }
    return stmt;
}

ProgNode * Parser::Parse() {
    ProgNode * prog = new ProgNode;  
    while (peek().has_value()) {
        auto stmt = ParseStmt();
        if (!stmt) {
            std::cerr << "error parsing stmt" << std::endl;
            exit(EXIT_FAILURE);
        }
        prog->stmts.push_back(stmt);
    }
    return prog;  
}
