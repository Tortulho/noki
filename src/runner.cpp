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
        std::cout << "ERROR: Critical error at parsing.\n";
        return RuntimeValue(nullptr);
    }

    // DEBUG
    // DUMP THE AST HERE
    // node->dump(4);

    EvalResult result = Interpreter::eval(*node);

    if (result.control != FlowControl::NONE)
    {
        throw std::runtime_error(
            "Invalid control flow outside of a valid context."
        );
    }

    return std::move(result.value);
}