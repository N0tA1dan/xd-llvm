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

            while(std::isalpha(peek().value())){
                buffer.push_back(eat());
            }

            if(m_TokenMap.contains(buffer) == true){
                tokens.push_back({m_TokenMap.at(buffer), std::nullopt});
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
            tokens.push_back({TokenType::INT_LIT, buffer});
            buffer.clear();
            continue;
        }

        // handles special characters
        else if (auto c = peek().value(); m_TokenMap.contains(std::string(1, c))) {
            tokens.push_back({ m_TokenMap.at(std::string(1, eat())), std::nullopt });
            buffer.clear();
            continue;
        }

        // ignore whitespace
        else if(isspace(peek().value())){
            eat();
        }

        else {
            std::cerr << "Unknown character: '" << peek().value() << "'\n";
            eat(); 
        }
    }

    return tokens;
}
