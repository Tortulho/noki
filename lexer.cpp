#include "lexer.hpp"
#include <vector>


/*

set x to value
let x to value

set x = 30
{SET_TOKEN, IDENTIFIER_TOKEN, EQUAL_TOKEN, INT_TOKEN}

x = 30
{IDENTIFIER_TOKEN, EQUAL_TOKEN, INT_TOKEN}

y1 = "abcdef"
{IDENTIFIER_TOKEN, EQUAL_TOKEN, STRING_TOKEN}

se tivermos um identifier token seguido de um equal token trata-se de uma atribuiçao
se houver o identifier no dictVar é uma atribuiçao à variavel
senao criar uma nova 



*/

Token::TokenType isReserved(std::string_view tokenstr) {

    for (size_t idx = 0; idx < sizeof(KEYWORDS) / sizeof(KEYWORD); idx++) {
        if (tokenstr == KEYWORDS[idx].keyword_string) return KEYWORDS[idx].type;
    }
    return Token::TokenType::IDENTIFIER;

}


void Token::tokenize(const std::string& input, std::vector<Token>& tokens) {

    size_t pos = 0;
    char c;

    while (pos < input.size()) {

        c = input[pos];

        if (isSkippable(c)) {
            pos++;
            continue;
        }

        //um char apenas
        switch (c) {
        //IF ONE-CHAR
        case '\n':
            tokens.push_back({"\n",1,Token::TokenType::NEWLINE_or_SEMICOLON});
            pos++;
            continue;
        case ';':
            tokens.push_back({";",1,Token::TokenType::NEWLINE_or_SEMICOLON});
            pos++;
            continue;
        case '#':
            while (pos < input.size() && input[pos] != '\n') pos++;
            continue;
        case '\0': //failsafe
            continue;
        case '(':
            tokens.push_back({"(",1,Token::TokenType::OPENPAR});
            pos++;
            continue;
        case ')':
            tokens.push_back({")",1,Token::TokenType::CLOSEPAR});
            pos++;
            continue;
        case '=':
            // pos++;
            // if (pos < input.size()) continue;
            // if (input[pos] == '=') tokens.push_back({"==",2,Token::TokenType::BINARY_OPERATOR});
            // else pos--;
            tokens.push_back({"=",1,Token::TokenType::EQUAL});
            pos++;
            continue;

        //IS BINARY OP

        case '+':
            tokens.push_back({"+",1,Token::TokenType::BINARY_OPERATOR});
            pos++;
            continue;
        case '-':
            tokens.push_back({"-",1,Token::TokenType::BINARY_OPERATOR});
            pos++;
            continue;
        case '*':
            tokens.push_back({"*",1,Token::TokenType::BINARY_OPERATOR});
            pos++;
            continue;
        case '/':
            tokens.push_back({"/",1,Token::TokenType::BINARY_OPERATOR});
            pos++;
            continue;
        case '%':
            tokens.push_back({"%",1,Token::TokenType::BINARY_OPERATOR});
            pos++;
            continue;
        case '^':
            tokens.push_back({"^",1,Token::TokenType::BINARY_OPERATOR});
            pos++;
            continue;
        case ',':
            tokens.push_back({",",1,Token::TokenType::COMMA});
            pos++;
            continue;

        //IS STRING
        case '"':
            size_t start = ++pos;
            while (pos < input.size() && input[pos] != '"') pos++;
            tokens.push_back({&input[start],pos-start,Token::TokenType::STRING});
            pos++;
            continue;
        }

        //MULTICHAR
        //is number
        if (isdigit(c)) {
            size_t start = pos;
            while (pos < input.size() && isdigit(input[pos])) pos++;

            if (pos < input.size() && input[pos] == '.') { //IS FLOAT
                pos++;
                while (pos < input.size() && isdigit(input[pos])) pos++;
                if (isalpha(input[pos])) tokens.push_back({"",INT64_MAX,Token::TokenType::null});
                tokens.push_back({&input[start], pos-start, Token::TokenType::FLOAT});
            }
            else {
                if (isalpha(input[pos])) tokens.push_back({"",INT64_MAX,Token::TokenType::null});
                tokens.push_back({&input[start],pos-start, Token::TokenType::INT});
            }

            continue;
        }
        //is identifier
        if (isalpha(c) || c == '_') {
            size_t start = pos;

            while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_')) pos++;

            //if reserved
            std::string_view word(input.data() + start, pos - start);
            Token::TokenType type = isReserved(word);
            
            tokens.push_back({&input[start], pos-start, type});

            continue;
        }


    }

    tokens.push_back({nullptr,0,Token::TokenType::END_OF_FILE});

    return;

}