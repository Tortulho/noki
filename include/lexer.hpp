#ifndef LEXER
#define LEXER

#include <memory>
#include <string>
#include <vector>
#include <string.h>
#include "utils.hpp"
#include <iostream>

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
            COLON,

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
            ELIF,
            WHILE,
            FOR,
            UNTIL,
            LET,
            CONTINUE,
            BREAK,
            TRY,
            CATCH,

            END_OF_FILE,

            //patch
            TRUE,
            FALSE,

            //file line to line - noki files
            NEWLINE_or_SEMICOLON,

            //scopes
            OPEN_SCOPE,
            CLOSE_SCOPE,

            IMPORT,
            DOT, //FOR FUNCS

            //vetores
            OPEN_BRACKET,
            CLOSE_BRACKET,

            FUNCTION,
            CONST,

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
    //{"int",         Token::TokenType::INT       },
    //{"float",       Token::TokenType::FLOAT     },
    //{"string",      Token::TokenType::STRING    },
    //{"bool",        Token::TokenType::BOOL      },
    {"null",        Token::TokenType::null          },
    {"return",      Token::TokenType::RETURN        },
    {"raise",       Token::TokenType::RAISE         },
    {"if",          Token::TokenType::IF            },
    {"else",        Token::TokenType::ELSE          },
    {"elif",        Token::TokenType::ELIF          },
    {"while",       Token::TokenType::WHILE         },
    {"for",         Token::TokenType::FOR           },
    {"until",       Token::TokenType::UNTIL         },
    {"let",         Token::TokenType::LET           },
    {"exit",        Token::TokenType::EXIT          },
    {"true",        Token::TokenType::BOOL          },
    {"false",       Token::TokenType::BOOL          },
    {"and",         Token::TokenType::BINARY_OPERATOR},
    {"or",          Token::TokenType::BINARY_OPERATOR},
    {"import",      Token::TokenType::IMPORT        },
    {"func",        Token::TokenType::FUNCTION      },
    {"function",    Token::TokenType::FUNCTION      },
    {"const",       Token::TokenType::CONST         },
    {"continue",       Token::TokenType::CONTINUE   },
    {"break",       Token::TokenType::BREAK         },
};

#endif