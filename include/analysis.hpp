#pragma once
#include "parser.hpp"
#include "lexer.hpp"
#include <memory>

struct SymbolInfo{
  TokenType type;
  bool intitialized;
};

class Analyzer{
  private:
    // vector works like stack in this case, we push and pop scopes accordingly
    std::vector<std::unordered_map<std::string, SymbolInfo>> m_scopes;
    std::vector<std::string> m_errors;


  public:
    Analyzer() = default; 
    

    void AnalyzePrimaryExpr(const std::unique_ptr<PrimaryExprNode>& primaryExpr);
    void AnalyzeExpr(const std::unique_ptr<ExprNode>& expr);
    void AnalyzeStmt(const std::unique_ptr<StmtNode>& stmt);
    bool Analyze(const std::unique_ptr<ProgNode>& prog);

};
