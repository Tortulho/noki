#include "dummy.hpp"

namespace dummy
{
    RuntimeValue hello(const std::vector<RuntimeValue>& args)
    {
        std::cout << "Hello from dummy library!\n";

        return RuntimeValue(nullptr);
    }
}

