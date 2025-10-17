#pragma once

#include <optional>
#include <vector>
#include <string>
#include <ostream>
#include <iostream>
#include <unordered_map>

enum class TokenType{
    EXIT,
    LET,
    IDENT,
    INT_LIT,
    STRING_LIT,
    EQUALS,
    SEMI,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    INT,
    FLOAT,
    CHAR,
    VOID,
    FN,
    ADD,
    SUB,
    MUL,
    DIV,
    RETURN,
};


struct Token{
    TokenType type;
    std::optional<std::string> value;
};


class Lexer{
    private:
        std::string m_code;
        int m_index = 0;

        std::unordered_map<std::string, TokenType> m_TokenMap = {
            {"exit", TokenType::EXIT},
            {"let", TokenType::LET},
            {"int", TokenType::INT},
            {"char", TokenType::CHAR},
            {"float", TokenType::FLOAT},
            {"void", TokenType::VOID},
            {"fn", TokenType::FN},
            {"return", TokenType::RETURN},
            {"+", TokenType::ADD},
            {"-", TokenType::SUB},
            {"/", TokenType::DIV},
            {"*", TokenType::MUL},
            {";", TokenType::SEMI},
            {"=", TokenType::EQUALS},
            {"(", TokenType::OPEN_PAREN},
            {")", TokenType::CLOSE_PAREN},
            {"{", TokenType::OPEN_BRACKET},
            {"}", TokenType::CLOSE_BRACKET},
        };

        std::optional<char> peek(int offset);
        char eat();

    public:
        Lexer(std::string code);

        std::vector<Token> lex();

        void print_tokens();
};
