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
    std::unique_ptr<ProgNode> m_prog;
    std::unordered_map<std::string, SymbolInfo> mSymbolTable;


  public:
    Analyzer(std::unique_ptr<ProgNode> prog) : m_prog(std::move(prog)) {}


};
