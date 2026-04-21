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
            case TokenType::SEMI:
                std::cerr << "Error, expected ';'" << std::endl;
                break;
            case TokenType::EQUAL:
                std::cerr << "Error, exptected '='" << std::endl;
                break;
        }
        exit(EXIT_FAILURE);
    }
}

std::unique_ptr<PrimaryExprNode> Parser::ParsePrimaryExpr(){
    auto primaryexpr = std::make_unique<PrimaryExprNode>();

    if(peek().has_value()){

        switch(peek().value().type){
            case TokenType::INT_LIT:
                {
                    auto intLit = std::make_unique<IntLitNode>();
                    intLit->val = eat();
                    primaryexpr->var = std::move(intLit);
                    break;
                }

            case TokenType::FLOAT_LIT:
                {
                    auto floatLit = std::make_unique<FloatLitNode>();
                    floatLit->val = eat();
                    primaryexpr->var = std::move(floatLit);
                    break;
                }

            case TokenType::IDENT:
                {
                    auto ident = std::make_unique<IdentNode>();
                    ident->val = eat();
                    primaryexpr->var = std::move(ident);
                    break;
                }

            case TokenType::OPEN_PAREN:
                {
                    eat();

                    auto innerExpr = ParseExpr();

                    TryEat(TokenType::CLOSE_PAREN);

                    primaryexpr->var = std::move(innerExpr);

                    break;
                }

            default:
                std::cerr << "Unexpected token type found while parsing primary expression" << std::endl;
                break;
        }
    }
    return primaryexpr;
}

std::unique_ptr<ExprNode> Parser::ParseFactor(){
    auto lhs = ParsePrimaryExpr();

    if(!lhs){
        std::cerr << "Error parsing primary expression in parse factor" << std::endl;
        exit(EXIT_FAILURE);
    } 

    auto lhsexpr = std::make_unique<ExprNode>();
    lhsexpr->var = std::move(lhs);

    bool isBinExpr = false;
    
    while(peek().has_value()){

        switch(peek().value().type){

            case TokenType::MUL:
                {
                    isBinExpr = true;
                    eat(); // eats * token
                    auto rhs = ParsePrimaryExpr();
                    auto rhsexpr = std::make_unique<ExprNode>();
                    rhsexpr->var = std::move(rhs);

                    auto binexpr = std::make_unique<BinOpExpr>();
                    binexpr->type = BinOpType::MUL;

                    binexpr->lhs = std::move(lhsexpr);
                    binexpr->rhs = std::move(rhsexpr);

                    auto newexpr = std::make_unique<ExprNode>();
                    newexpr->var = std::move(binexpr);
                    lhsexpr = std::move(newexpr);

                    break;
                }

            case TokenType::DIV:
                {
                    isBinExpr = true;
                    eat(); // eats / token
                    auto rhs = ParsePrimaryExpr();
                    auto rhsexpr = std::make_unique<ExprNode>();
                    rhsexpr->var = std::move(rhs);

                    auto binexpr = std::make_unique<BinOpExpr>();
                    binexpr->type = BinOpType::DIV;

                    binexpr->lhs = std::move(lhsexpr);
                    binexpr->rhs = std::move(rhsexpr);

                    auto newexpr = std::make_unique<ExprNode>();
                    newexpr->var = std::move(binexpr);
                    lhsexpr = std::move(newexpr);

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

std::unique_ptr<ExprNode> Parser::ParseTerm(){

    auto lhs = ParseFactor();

    if(!lhs){
        std::cerr << "Error parsing primary expression in parse factor" << std::endl;
        exit(EXIT_FAILURE);
    } 

    auto lhsexpr = std::move(lhs);

    bool isBinExpr = false;
    
    while(peek().has_value()){

        switch(peek().value().type){

            // left off here
            case TokenType::ADD:
                {
                    isBinExpr = true;
                    eat(); // eats + token
                    auto rhs = ParseFactor();
                    auto rhsexpr = std::make_unique<ExprNode>();
                    rhsexpr = std::move(rhs);

                    auto binexpr = std::make_unique<BinOpExpr>(); 
                    binexpr->type = BinOpType::ADD;

                    binexpr->lhs = std::move(lhsexpr);
                    binexpr->rhs = std::move(rhsexpr);

                    auto newexpr = std::make_unique<ExprNode>();
                    newexpr->var = std::move(binexpr);
                    lhsexpr = std::move(newexpr);

                    break;
                }

            case TokenType::SUB:
                {
                    isBinExpr = true;
                    eat(); // eats + token
                    auto rhs = ParseFactor();
                    auto rhsexpr = std::make_unique<ExprNode>();
                    rhsexpr = std::move(rhs);

                    auto binexpr = std::make_unique<BinOpExpr>(); 
                    binexpr->type = BinOpType::SUB;

                    binexpr->lhs = std::move(lhsexpr);
                    binexpr->rhs = std::move(rhsexpr);

                    auto newexpr = std::make_unique<ExprNode>();
                    newexpr->var = std::move(binexpr);
                    lhsexpr = std::move(newexpr);

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

std::unique_ptr<ExprNode> Parser::ParseComparison(){
    auto lhs = ParseTerm();

    if(!lhs){
        std::cerr << "Erorr parsing term in parse comparison" << std::endl;
        exit(EXIT_FAILURE);
    }

    auto lhsExpr = std::move(lhs);


    bool isConditional = false;

    while(peek().has_value()){
        ConditionalOpType opType;

        switch(peek().value().type){
            case TokenType::LESS_THAN:
                opType = ConditionalOpType::LESS_THAN;
                isConditional = true;
                eat();
                break;

            case TokenType::GREATER_THAN:
                opType = ConditionalOpType::GREATER_THAN;
                isConditional = true;
                eat();
                break;

            case TokenType::LESS_OR_EQUAL:
                opType = ConditionalOpType::LESS_OR_EQUAL;
                isConditional = true;
                eat();
                break;

            case TokenType::GREATER_OR_EQUAL:
                opType = ConditionalOpType::GREATER_OR_EQUAL;
                isConditional = true;
                eat();
                break;

            default:
                isConditional = false;
                break;
        }

        if(isConditional == true){
            
            auto rhsExpr = ParseTerm(); 

            auto conditionalOp = std::make_unique<ConditionalOpExpr>();

            conditionalOp->type = opType;
            conditionalOp->lhs= std::move(lhsExpr);
            conditionalOp->rhs= std::move(rhsExpr);

            auto newExpr = std::make_unique<ExprNode>();
            newExpr->var = std::move(conditionalOp);
            lhsExpr = std::move(newExpr);
            
        }

        if(isConditional == false) break;

    }

    return lhsExpr;
}

std::unique_ptr<ExprNode> Parser::ParseEquality(){
    auto lhs = ParseComparison();

    if(!lhs){
        std::cerr << "Erorr parsing term in parse comparison" << std::endl;
        exit(EXIT_FAILURE);
    }

    auto lhsExpr = std::move(lhs);


    bool isConditional = false;

    while(peek().has_value()){
        ConditionalOpType opType;

        switch(peek().value().type){
            case TokenType::EQUAL_TO:
                opType = ConditionalOpType::EQUAL_TO;
                isConditional = true;
                eat();
                break;

            case TokenType::NOT_EQUAL:
                opType = ConditionalOpType::NOT_EQUAL;
                isConditional = true;
                eat();
                break;

            default:
                isConditional = false;
                break;
        }

        if(isConditional == true){
            
            auto rhsExpr = ParseComparison(); 

            auto conditionalOp = std::make_unique<ConditionalOpExpr>();

            conditionalOp->type = opType;
            conditionalOp->lhs= std::move(lhsExpr);
            conditionalOp->rhs= std::move(rhsExpr);

            auto newExpr = std::make_unique<ExprNode>();
            newExpr->var = std::move(conditionalOp);
            lhsExpr = std::move(newExpr);
            
        }

        if(isConditional == false) break;
    }

    return lhsExpr;
}


std::unique_ptr<ExprNode> Parser::ParseExpr(){
    auto expr = ParseEquality();
    return expr;

}

std::unique_ptr<ProtoTypeNode> Parser::ParseProto(){
    auto proto = std::make_unique<ProtoTypeNode>();
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

std::unique_ptr<FunctionNode> Parser::ParseFunc(){
    auto prototype = ParseProto(); 
    if(!prototype){
        std::cerr<<"error parsing prototype for function" << std::endl;
        exit(EXIT_FAILURE);
    }
    auto func = std::make_unique<FunctionNode>();
    func->prototype = std::move(prototype);

    TryEat(TokenType::OPEN_BRACKET);

    while(peek().has_value() && peek().value().type != TokenType::CLOSE_BRACKET){
        func->body.push_back(ParseStmt());
    }
    TryEat(TokenType::CLOSE_BRACKET);
    return func;
}

std::unique_ptr<CompoundStmtNode> Parser::ParseCompoundStmt(){
  std::unique_ptr<CompoundStmtNode> compoundStmt = std::make_unique<CompoundStmtNode>();

  while(peek().has_value() && peek().value().type != TokenType::CLOSE_BRACKET){
      compoundStmt->body.push_back(ParseStmt());

  }

  TryEat(TokenType::CLOSE_BRACKET);

  return compoundStmt;

}
std::unique_ptr<DeclerationStmtNode> Parser::ParseDecleration(){
    auto decleration = std::make_unique<DeclerationStmtNode>();

    decleration->type = eat();

    decleration->identifier = eat();


    if(peek().has_value() && peek().value().type == TokenType::EQUAL){
        TryEat(TokenType::EQUAL);
        decleration->expression = ParseExpr();
    }

    TryEat(TokenType::SEMI);

    return decleration;

}

std::unique_ptr<AssignmentNode> Parser::ParseAssignmentStmt(){
    auto assignment = std::make_unique<AssignmentNode>();

    assignment->identifier = eat(); // eats identifier 

    TryEat(TokenType::EQUAL);

    assignment->expression = ParseExpr();

    TryEat(TokenType::SEMI);

    return assignment;
}

std::unique_ptr<IfStmtNode> Parser::ParseIfStmt(){

    auto ifStmt = std::make_unique<IfStmtNode>();

    TryEat(TokenType::OPEN_PAREN);  

    ifStmt->condition = ParseExpr();

    TryEat(TokenType::CLOSE_PAREN);

    TryEat(TokenType::OPEN_BRACKET);

    // parse if statement body statements
    while(peek().has_value() && peek().value().type != TokenType::CLOSE_BRACKET){
        ifStmt->thenBody.push_back(ParseStmt());

    }

    TryEat(TokenType::CLOSE_BRACKET);

    // parse else body
    if(peek().has_value() && peek().value().type == TokenType::ELSE){
        TryEat(TokenType::ELSE);
        TryEat(TokenType::OPEN_BRACKET);
        while(peek().has_value() && peek().value().type != TokenType::CLOSE_BRACKET){

            ifStmt->elseBody.push_back(ParseStmt());
        }
        TryEat(TokenType::CLOSE_BRACKET);
    }
    

    return ifStmt;

}

std::unique_ptr<StmtNode> Parser::ParseStmt(){
    auto stmt = std::make_unique<StmtNode>();

    auto current = peek();
    if (!current.has_value()) {
        std::cerr << "error: unexpected end of input while parsing statement" << std::endl;
        exit(EXIT_FAILURE);
    }

    // compound statements
    if(peek().value().type == TokenType::OPEN_BRACKET){
      eat(); // eats open bracket
      auto compoundStmt = ParseCompoundStmt();

      if(!compoundStmt){
        std::cerr << "error: couldnt parse compoundStmt" << std::endl;
        exit(EXIT_FAILURE);
      }

      stmt->var = std::move(compoundStmt);
    }

    // handles functions
    if(peek().value().type == TokenType::FN){
        eat(); // eat fn token
        auto func = ParseFunc();
        if(!func){
            std::cerr << "error parsing function" << std::endl;
            exit(EXIT_FAILURE);
        }
        stmt->var = std::move(func);
        
    }

    else if(peek().value().type == TokenType::INT || peek().value().type == TokenType::FLOAT || peek().value().type == TokenType::UINT){

        auto decleration = ParseDecleration();

        if(!decleration){
            std::cerr << "error parsing let statement" << std::endl;
            exit(EXIT_FAILURE);
        }
        stmt->var = std::move(decleration);
    }


    else if (peek().value().type == TokenType::IDENT) {

        auto next = peek(1);

        if (next.has_value() == false) {
            std::cerr << "Error: unexpected end of input after identifier" << std::endl;
            exit(EXIT_FAILURE);
        }

        // assigment statement
        if (next->type == TokenType::EQUAL) {
            auto assignment = ParseAssignmentStmt();
            if (!assignment) {
                std::cerr << "error parsing assignment" << std::endl;
                exit(EXIT_FAILURE);
            }
            stmt->var = std::move(assignment);
        }

        // function call
        else if (next->type == TokenType::OPEN_PAREN) {
        }
    }

    else if(peek().value().type == TokenType::IF){
        eat(); // eats if token
        auto ifStmt = ParseIfStmt();

        if(!ifStmt){
            std::cerr << "Error parsing if statement" << std::endl;
            exit(EXIT_FAILURE);
        }
        stmt->var = std::move(ifStmt);
    }

    return stmt;
}

std::unique_ptr<ProgNode> Parser::Parse() {
    auto prog = std::make_unique<ProgNode>();  
    while (peek().has_value()) {
        auto stmt = ParseStmt();
        if (!stmt) {
            std::cerr << "error parsing stmt" << std::endl;
            exit(EXIT_FAILURE);
        }
        prog->stmts.push_back(std::move(stmt));
    }
    return prog;  
}
