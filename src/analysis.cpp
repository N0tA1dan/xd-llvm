#include "analysis.hpp"

void Analyzer::AnalyzeStmt(const std::unique_ptr<StmtNode>& stmt){
  struct StmtVisitor{
    Analyzer& self;

    // handles type checking and checks if variables exist
    void operator()(const std::unique_ptr<AssignmentNode>& assignment){
      return;
    }

    void operator()(const std::unique_ptr<IfStmtNode>& ifstmt){
      return;
    }

    void operator()(const std::unique_ptr<FunctionNode>& function){
      // create a new scope?
      for(const auto& stmt : function->body){
        self.AnalyzeStmt(stmt);
      }
      return;
    }

    void operator()(const std::unique_ptr<DeclerationStmtNode>& decleration){
      // check map if decleration already exists if not add it to symbols
      auto variableName = decleration->identifier.value.value();

      if(self.m_symbols.find(variableName) != self.m_symbols.end()){
        self.m_errors.push_back("error: redecleration of variable " + variableName + '\n');
        
      }else{
        self.m_symbols.insert({variableName, {decleration->type.type, true}});
      }

      return;
    }

  };

  StmtVisitor visitor = {*this};
  std::visit(visitor, stmt->var);

}

bool Analyzer::Analyze(){
  for(const auto& stmt : m_prog->stmts){
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
