#pragma once

#include <optional>
#include <vector>
#include <string>
#include <ostream>
#include <iostream>

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

        std::optional<char> peek(int offset);
        char eat();

    public:
        Lexer(std::string code);

        std::vector<Token> lex();
};
