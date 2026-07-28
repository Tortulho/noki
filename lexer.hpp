#ifndef LEXER
#define LEXER

#include <memory>
#include <string>
#include <vector>
#include <string.h>
#include "utils.hpp"
#include <iostream>


// class Keyword {
//     public:
//         typedef enum {

//         } KEYWORDS;
// };



class Token {

    public:
        typedef enum {         
            
            EMPTY,
            END,
            INVALID,

            IDENTIFIER,

            EQUAL,
            PLUS,
            MINUS,
            MULT,
            DIV,
            MOD,
            POW,

            OPENPAR, CLOSEPAR,
            COMMA,

            UNARY_OPERATOR,
            BINARY_OPERATOR,

            INT,
            FLOAT,
            STRING,
            BOOL,
            null,
            
            RETURN,
            RAISE,
            IF,
            ELSE,
            WHILE,
            FOR,
            UNTIL,
            LET,

            END_OF_FILE,

            //patch
            TRUE,
            FALSE,

            //file line to line - noki files
            NEWLINE_or_SEMICOLON,

            //scopes
            OPEN_SCOPE,
            CLOSE_SCOPE,

            EXIT,

        } TokenType;

    private:
        const char *start;
        size_t len;
        TokenType type;

    public:
        static void tokenize(const std::string& input, std::vector<Token>& tokens);

        Token(const char *start, size_t len, TokenType Type)
        : start(start), len(len), type(Type) {}
        
        TokenType getType() {return type;}
        std::string_view getString() const {
            return std::string_view(start, len);
        }

};

typedef struct {
    std::string keyword_string;
    Token::TokenType type;
} KEYWORD;

const KEYWORD KEYWORDS[] = {
    {"int",         Token::TokenType::INT       },
    {"float",       Token::TokenType::FLOAT     },
    {"string",      Token::TokenType::STRING    },
    {"bool",        Token::TokenType::BOOL      },
    {"null",        Token::TokenType::null      },
    {"return",      Token::TokenType::RETURN    },
    {"raise",       Token::TokenType::RAISE     },
    {"if",          Token::TokenType::IF        },
    {"else",        Token::TokenType::ELSE      },
    {"while",       Token::TokenType::WHILE     },
    {"for",         Token::TokenType::FOR       },
    {"until",       Token::TokenType::UNTIL     },
    {"let",         Token::TokenType::LET       },
    {"exit",        Token::TokenType::EXIT      },
    {"true",        Token::TokenType::BOOL      },
    {"false",       Token::TokenType::BOOL      },
    {"and",    Token::TokenType::BINARY_OPERATOR},
    {"or",     Token::TokenType::BINARY_OPERATOR},
};

#endif