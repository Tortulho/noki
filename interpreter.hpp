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
        static RuntimeValue evalUni(const ASTNode& node);
        static RuntimeValue evalBlock(const ASTNode& node);
        static RuntimeValue evalIf(const ASTNode& node);
        static RuntimeValue evalWhile(const ASTNode& node);
        static RuntimeValue evalImport(const ASTNode& node);
        static RuntimeValue evalMemberAccess(const ASTNode& node);
        static RuntimeValue evalVector(const ASTNode& node);
        static RuntimeValue evalIndex(const ASTNode& node);
        static RuntimeValue* resolveLValue(const ASTNode& node);

};