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
    IF,
    INT_LIT,
    FLOAT_LIT,
    STRING_LIT,
    EQUAL,
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
    ADD_EQ,
    SUB_EQ,
    MUL_EQ,
    DIV_EQ,
    EQUAL_TO,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_OR_EQUAL,
    GREATER_OR_EQUAL,
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
            {"if", TokenType::IF},
            {"+", TokenType::ADD},
            {"-", TokenType::SUB},
            {"/", TokenType::DIV},
            {"*", TokenType::MUL},
            {";", TokenType::SEMI},
            {"=", TokenType::EQUAL},
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
