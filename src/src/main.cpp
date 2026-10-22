#include "ast.hpp"
#include "lexer.hpp"
#include <memory>
#include <vector>
#include "parser.hpp"
#include "interpreter.hpp"
#include "runtimeval.hpp"
#include "runtime.hpp"
#include "runner.hpp"
#include <fstream>
#include <iterator>

int main(int argc, char const *argv[])
{
    std::cout << "Noki REPL v1.0 'Clear exiting'\n";

    if (argc == 1) {
        //MODO REPL
        while (true)
        {
            std::string line;
            std::getline(std::cin, line);

            EvalResult result = Runner::run(line);

            if (result.control == FlowControl::EXIT)
            {
                while (runtime.current != &runtime.global)
                {
                    runtime.popEnvironment();
                }

                return EXIT_SUCCESS;
            }

            std::cout << "(if applicable) Result of operation is: ";
            result.value.print();
            std::cout << "\n";
        }
    } else {
        //MODO LER DE UM FILE
        std::ifstream file(argv[1]);

        std::string source{
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        };

        EvalResult result = Runner::run(source);

        if (result.control == FlowControl::EXIT)
        {
            while (runtime.current != &runtime.global)
            {
                runtime.popEnvironment();
            }

            return EXIT_SUCCESS;
        }

        std::cout << "Program executed.\n";

        return EXIT_SUCCESS;
    }
}
