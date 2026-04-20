#include "analysis.hpp"

void Analyzer::AnalyzeExpr(const std::unique_ptr<ExprNode>& expr){
  struct ExprVisitor{
    Analyzer& self;

      void operator()(const std::unique_ptr<PrimaryExprNode>& primaryExpr){
        return;
      }

      void operator()(const std::unique_ptr<BinOpExpr>& binExpr){
        return;
      }

      void operator()(const std::unique_ptr<ConditionalOpExpr>& conditionalExpr){
        return;
      }
  };

  std::visit(ExprVisitor{*this}, expr->var);

}

void Analyzer::AnalyzeStmt(const std::unique_ptr<StmtNode>& stmt){
  struct StmtVisitor{
    Analyzer& self;

    // handles type checking and checks if variables exist and if its initialized
    void operator()(const std::unique_ptr<AssignmentNode>& assignment){
      auto variableName = assignment->identifier.value.value();

      if(self.m_scopes.back().find(variableName) == self.m_scopes.back().end()){
        self.m_errors.push_back("error: variable '" + variableName + "' was not declared in this scope \n");
      }
      return;
    }

    void operator()(const std::unique_ptr<IfStmtNode>& ifstmt){
      return;
    }

    void operator()(const std::unique_ptr<FunctionNode>& function){
      // create a new scope?
      self.m_scopes.push_back({});
      for(const auto& stmt : function->body){
        self.AnalyzeStmt(stmt);
      }
      return;
    }

    void operator()(const std::unique_ptr<DeclerationStmtNode>& decleration){
      // check map if decleration already exists if not add it to symbols
      auto variableName = decleration->identifier.value.value();

      if(self.m_scopes.back().find(variableName) != self.m_scopes.back().end()){
        self.m_errors.push_back("error: redecleration of variable " + variableName + '\n');
        
      }else{
        self.m_scopes.back().insert({variableName, {decleration->type.type, true}});
      }

      return;
    }

  };

  StmtVisitor visitor = {*this};
  std::visit(visitor, stmt->var);

}

bool Analyzer::Analyze(const std::unique_ptr<ProgNode>& prog){
  for(const auto& stmt : prog->stmts){
    AnalyzeStmt(stmt);
  }

  if(m_errors.size() > 0){
    for(const auto& error: m_errors){
      std::cerr << error;
    }
    
    return false;
  }
  return true;
}
