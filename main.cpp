#include "ast.hpp"
#include "lexer.hpp"
#include <memory>
#include <vector>
#include "parser.hpp"
#include "interpreter.hpp"
#include "runtime/runtimeval.hpp"
#include "runner.hpp"
#include <fstream>
#include <iterator>

int main(int argc, char const *argv[])
{
    std::cout << "Noki REPL v0.2 Tech: 'Running from file'\n";

    if (argc == 1) {
        //MODO REPL
        while (true)
        {
            std::string line;
            std::getline(std::cin, line);

            RuntimeValue result = Runner::run(line);

            std::cout << "Result of operation is: ";
            result.print();
        }
    } else {
        //MODO LER DE UM FILE
        std::ifstream file(argv[1]);

        std::string source{
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        };

        RuntimeValue result = Runner::run(source);

        // std::cout << "Result of operation: ";
        // result.print();
        return EXIT_SUCCESS;
    }
}
