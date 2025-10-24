#include "lexer.hpp"
#include "parser.hpp"
#include "generator.hpp"
#include <fstream>
#include <sstream>

void print_tokens(const std::vector<Token> & tokens){
    for(auto token : tokens){
        switch(token.type){
            case TokenType::LET:
                std::cout << "LET TOKEN" << std::endl;
                break;
                
            case TokenType::EXIT:
                std::cout << "EXIT TOKEN" << std::endl;

            case TokenType::INT:
                std::cout << "INT TOKEN" << std::endl;
                break;

            case TokenType::CHAR:
                std::cout << "CHAR TOKEN" << std::endl;
                break;

            case TokenType::VOID:
                std::cout << "VOID TOKEN" << std::endl;
                break;

            case TokenType::FLOAT:
                std::cout << "FLOAT TOKEN" << std::endl;
                break;

            case TokenType::IDENT:
                std::cout << "IDENT TOKEN: " << token.value.value()<< std::endl;

                break;

            case TokenType::INT_LIT:
                std::cout << "INT_LIT TOKEN" << std::endl;
                break;
            
            case TokenType::FLOAT_LIT:
                std::cout << "FLOAT_LIT TOKEN"<<std::endl;
                break;

            case TokenType::EQUAL:
                std::cout << "EQUAL TOKEN" << std::endl;
                break;

            case TokenType::SEMI:
                std::cout << "SEMI TOKEN" << std::endl;
                break;

            case TokenType::FN:
                std::cout << "FN TOKEN" << std::endl;
                break;

            case TokenType::OPEN_PAREN:
                std::cout << "OPEN_PAREN TOKEN" << std::endl;
                break;

            case TokenType::CLOSE_PAREN:
                std::cout << "CLOSE_PAREN TOKEN" << std::endl;
                break;

            case TokenType::OPEN_BRACKET:
                std::cout << "OPEN_BRACKET TOKEN" << std::endl;
                break;

            case TokenType::CLOSE_BRACKET:
                std::cout << "CLOSE_BRACKET TOKEN" << std::endl;
                break;

            case TokenType::ADD_EQ:
                std::cout << "ADD_EQ TOKEN" << std::endl;
                break;
            
            case TokenType::EQUAL_TO:
                std::cout << "EQUAL_TO TOKEN" << std::endl;
                break;
        }
    }
}

int main(int argc, char * argv[]){

    std::ifstream t(argv[1]);
    std::stringstream buffer;
    buffer << t.rdbuf();

    Lexer lex(buffer.str());
    std::vector<Token> tokens = lex.lex();
    print_tokens(tokens);

    Parser parser(tokens);
    auto prog = parser.Parse();

    Generator generator(prog);
    
    generator.Generate();

    return 0;
}
