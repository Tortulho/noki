#include "runner.hpp"

#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

RuntimeValue Runner::run(const std::string& source) {
    
    std::vector<Token> tokens;
    Token::tokenize(source, tokens);

    // DEBUG
    std::cout << "DEBUG - PRINT TOKENS HERE WITH DEBUGGER\n";

    Parser parser(tokens);

    auto node = parser.parse();

    if (node == nullptr) {
        std::cout << "ERROR: error at parsing.\n";
        return RuntimeValue(nullptr);
    }

    // DEBUG
    //std::cout << "DEBUG - PRINT NODE HERE WITH DEBUGGER\n";
    node->dump(4);

    RuntimeValue result = Interpreter::eval(*node);

    return result;
}