#include "runtime/runtimeval.hpp"
#include "runtime/runtime.hpp"
#include "ast.hpp"

class Interpreter {

    public:
        static RuntimeValue eval(const ASTNode& node);
    private:
        static RuntimeValue evalBinary(const ASTNode& node);
        static RuntimeValue evalAssign(const ASTNode& node);
        static RuntimeValue evalVar(const ASTNode& node);
        static RuntimeValue evalFuncCall(const ASTNode& node);

};