#include "lexer.hpp"

Lexer::Lexer(std::string code) : m_code(code) {}

[[nodiscard]] std::optional<char> Lexer::peek(int offset = 0){
    if(m_index + offset >= m_code.length()){
        return std::nullopt;
    } else{
        return m_code.at(m_index + offset);
    }
}

char Lexer::eat(){
    return m_code.at(m_index++);
}

std::vector<Token> Lexer::lex(){
    std::vector<Token> tokens;
    std::string buffer;

    while(peek().has_value()){
        // processes keywords and identifiers
        if(std::isalpha(peek().value())){
            buffer.push_back(eat());

            while(std::isalpha(peek().value())){
                buffer.push_back(eat());
            }

            if(buffer == "let"){
                tokens.push_back({TokenType::LET, std::nullopt});
                buffer.clear();
                continue;
            }

            else if(buffer == "exit"){
                tokens.push_back({TokenType::EXIT, std::nullopt});
                buffer.clear();
                continue;
            }

            else if(buffer == "int"){
                tokens.push_back({TokenType::INT, std::nullopt});
                buffer.clear();
                continue;
            }

            else if(buffer == "char"){
                tokens.push_back({TokenType::CHAR, std::nullopt});
                buffer.clear();
                continue;
            }

            else if(buffer == "float"){
                tokens.push_back({TokenType::FLOAT, std::nullopt});
                buffer.clear();
                continue;
            }

            else if(buffer == "void"){
                tokens.push_back({TokenType::VOID, std::nullopt});
                buffer.clear();
                continue;
            }

            else if(buffer  == "fn"){
                tokens.push_back({TokenType::FN, std::nullopt});
                buffer.clear();
                continue;
            }

            else if(buffer == "return"){
                tokens.push_back({TokenType::RETURN, std::nullopt});
                buffer.clear();
                continue;
            }

            // otherwise treat as identifier
            tokens.push_back({TokenType::IDENT, buffer});
            buffer.clear();
        }

        else if(isdigit(peek().value())){
            buffer.push_back(eat());

            while(isdigit(peek().value())){
                buffer.push_back(eat());
            }
            tokens.push_back({TokenType::INT_LIT, buffer});
            buffer.clear();
            continue;
        }

        else if(peek().value() == '='){
            eat();
            tokens.push_back({TokenType::EQUALS, std::nullopt});
            buffer.clear();
            continue;
        }

        else if(peek().value() == '('){
            eat();
            tokens.push_back({TokenType::OPEN_PAREN, std::nullopt});
            buffer.clear();
            continue;
        }


        else if(peek().value() == ')'){
            eat();
            tokens.push_back({TokenType::CLOSE_PAREN, std::nullopt});
            buffer.clear();
            continue;
        }

        else if(peek().value() == '{'){
            eat();
            tokens.push_back({TokenType::OPEN_BRACKET, std::nullopt});
            buffer.clear();
            continue;
        }

        else if(peek().value() == '}'){
            eat();
            tokens.push_back({TokenType::CLOSE_BRACKET, std::nullopt});
            buffer.clear();
            continue;
        }

        else if(peek().value() == ';'){
            eat();
            tokens.push_back({TokenType::SEMI, std::nullopt});
            buffer.clear();
            continue;
        }
        
        else if(peek().value() == '+'){
            eat();
            tokens.push_back({TokenType::ADD, std::nullopt});
            buffer.clear();
            continue;
        }

        else if(peek().value() == '-'){
            eat();
            tokens.push_back({TokenType::SUB, std::nullopt});
            buffer.clear();
            continue;
        }

        else if(peek().value() == '*'){
            eat();
            tokens.push_back({TokenType::MUL, std::nullopt});
            buffer.clear();
            continue;
        }

        else if(peek().value() == '/'){
            eat();
            tokens.push_back({TokenType::DIV, std::nullopt});
            buffer.clear();
            continue;
        }
        
        // ignore whitespace
        else if(isspace(peek().value())){
            eat();
        }
    }

    return tokens;
}
