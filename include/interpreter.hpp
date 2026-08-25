#include "runtime/runtimeval.hpp"
#include "runtime/runtime.hpp"
#include "ast.hpp"

enum class FlowControl
{
    NONE,
    BREAK,
    CONTINUE,
    RETURN,
    INVALID,
};

struct EvalResult
{
    RuntimeValue value;
    FlowControl control = FlowControl::NONE;
};

class Interpreter {

    public:
        static EvalResult  eval(const ASTNode& node);
    private:
        static EvalResult  evalBinary(const ASTNode& node);
        static EvalResult  evalAssign(const ASTNode& node);
        static EvalResult  evalVar(const ASTNode& node);
        static EvalResult  evalFuncCall(const ASTNode& node);
        static EvalResult  evalUni(const ASTNode& node);
        static EvalResult  evalBlock(const ASTNode& node);
        static EvalResult  evalIf(const ASTNode& node);
        static EvalResult  evalWhile(const ASTNode& node);
        static EvalResult  evalImport(const ASTNode& node);
        static EvalResult  evalMemberAccess(const ASTNode& node);
        static EvalResult  evalVector(const ASTNode& node);
        static EvalResult  evalIndex(const ASTNode& node);
        static RuntimeValue* resolveLValue(const ASTNode& node);
        static EvalResult  evalFuncDecl(const ASTNode& node);

};