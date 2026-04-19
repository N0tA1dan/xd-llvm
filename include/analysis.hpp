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
    std::unordered_map<std::string, SymbolInfo> m_symbols;
    std::vector<std::unordered_map<std::string, SymbolInfo>> m_scopes;
    std::vector<std::string> m_errors;


  public:
    Analyzer() = default; 
    

    void AnalyzeStmt(const std::unique_ptr<StmtNode>& stmt);
    bool Analyze(const std::unique_ptr<ProgNode>& prog);

};
