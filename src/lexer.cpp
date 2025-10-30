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

    // main loop to cover all characters within the file
    while(peek().has_value()){

        // processes keywords and identifiers
        if(std::isalpha(peek().value())){
            buffer.push_back(eat());

            while(std::isalnum(peek().value()) || peek().value() == '_' ){
                buffer.push_back(eat());
            }

            if(m_TokenMap.contains(buffer) == true){
                tokens.push_back({m_TokenMap.at(buffer), std::nullopt});
                buffer.clear();
                continue;
            }

            // otherwise treat as identifier
            tokens.push_back({TokenType::IDENT, buffer});
            buffer.clear();
        }

        // processes numerical values
        else if(isdigit(peek().value())){
            buffer.push_back(eat());

            while(isdigit(peek().value())){
                buffer.push_back(eat());
            }
            
            // handles floating point
            if(peek().has_value() && peek().value() == '.'){
                buffer.push_back(eat());

                while(isdigit(peek().value())){
                    buffer.push_back(eat());
                }

                tokens.push_back({TokenType::FLOAT_LIT, buffer});
                buffer.clear();
            } else {
                tokens.push_back({TokenType::INT_LIT, buffer});
                buffer.clear();
            }

            continue;
        }

        // handles whitespace 
        else if(isspace(peek().value())){
            eat();
            continue;
        }

        else if(peek().value()){
            TokenType type;

            switch(peek().value()){
                case '+':
                    if(peek(1).value() == '=') {
                        type = TokenType::ADD_EQ;
                        eat();
                    } else{
                        type = TokenType::ADD;
                    }
                    break;

                case '-':
                    if(peek(1).value() == '=') {
                        type = TokenType::SUB_EQ;
                        eat();
                    } else{
                        type = TokenType::SUB;
                    }
                    break;

                case '*':
                    if(peek(1).value() == '=') {
                        type = TokenType::MUL_EQ;
                        eat();
                    } else{
                        type = TokenType::MUL;
                    }
                    break;

                case '=':
                    if(peek(1).value() == '='){
                        type = TokenType::EQUAL_TO;
                        eat();
                    } else{
                        type = TokenType::EQUAL;
                    }
                    break;

                case '>':
                    if(peek(1).value() == '='){
                        type = TokenType::GREATER_OR_EQUAL;
                        eat();
                    }else{
                        type = TokenType::GREATER_THAN;
                    }
                    break;

                case '<':
                    if(peek(1).value() == '='){
                        type = TokenType::LESS_OR_EQUAL;
                        eat();
                    }else{
                        type = TokenType::LESS_THAN;
                    }
                    break;
                
                case '(':
                    type = TokenType::OPEN_PAREN;
                    break;
                
                case ')':
                    type = TokenType::CLOSE_PAREN;
                    break;

                case '{':
                    type = TokenType::OPEN_BRACKET;
                    break;
                
                case '}':
                    type = TokenType::CLOSE_BRACKET;
                    break;
                
                case ';':
                    type = TokenType::SEMI;
                    break;

                default:
                    std::cerr<< "Lexer Error: Unknown character detected " << peek().value() << std::endl;
                    break;
            }

            tokens.push_back({type, std::nullopt});
            eat();
            buffer.clear();
            continue;
        }
       

        else {
            std::cerr << "Unknown character: '" << peek().value() << std::endl;
            eat(); 
        }
    }

    return tokens;
}
