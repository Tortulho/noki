#ifndef AST
#define AST

#include "lexer.hpp"
#include "runtimefunc.hpp"
#include <memory>
#include <variant>
#include "asttypes.hpp"

using ASTValue = std::variant<
    int64_t,
    double,
    bool,
    void*,
    std::string_view,
    std::unique_ptr<std::vector<FunctionParameter>>
>;

class ASTNode {

    public:
        using TypeNode = ::TypeNode;
        
        enum TypeOp{
            NONE,

            // Matemáticos
            ADD,
            SUB,
            MUL,
            DIV,
            MOD,
            POW,

            // Comparação
            EQUAL,
            NOT_EQUAL,
            LESS,
            LESS_EQUAL,
            GREATER,
            GREATER_EQUAL,

            // Lógicos
            AND,
            OR,
            NOT, //NOT USED, -> NEGATIVE

            // Unários
            POSITIVE,
            NEGATIVE,
        };

        ASTValue as;
        TypeNode type;

    private:

    public:

        //TypeNode type;
        TypeOp op = TypeOp::NONE;

        std::vector<std::unique_ptr<ASTNode>> children;

        static std::unique_ptr<ASTNode> ast_newInt(int64_t ASTValue);
        static std::unique_ptr<ASTNode> ast_newFloat(double ASTValue);
        static std::unique_ptr<ASTNode> ast_newString(std::string_view text);
        static std::unique_ptr<ASTNode> ast_newBool(bool ASTValue);
        static std::unique_ptr<ASTNode> ast_newVar(std::string_view text);
        static std::unique_ptr<ASTNode> ast_newBinary(TypeOp op,
                                    std::unique_ptr<ASTNode> left,
                                    std::unique_ptr<ASTNode> right);
        static std::unique_ptr<ASTNode> ast_newUnary(TypeOp op,
                                    std::unique_ptr<ASTNode> operand);
        static std::unique_ptr<ASTNode> ast_newAssignment(
                                    std::unique_ptr<ASTNode> variable,
                                    std::unique_ptr<ASTNode> expression);
        static std::unique_ptr<ASTNode> ast_newFunctionCall(
                                    std::unique_ptr<ASTNode> function);       
        static std::unique_ptr<ASTNode> ast_newNULL();                 
        static std::unique_ptr<ASTNode> ast_newProgram(); 
        static std::unique_ptr<ASTNode> ast_newBlock(); 
        static std::unique_ptr<ASTNode> ast_newIf(
            std::unique_ptr<ASTNode> condition,
            std::unique_ptr<ASTNode> trueBlock,
            std::unique_ptr<ASTNode> falseBlock); 
        static std::unique_ptr<ASTNode> ast_newWhile(
            std::unique_ptr<ASTNode> condition,
            std::unique_ptr<ASTNode> block);
        static std::unique_ptr<ASTNode> ast_newImport(std::string_view name);
        static std::unique_ptr<ASTNode> ast_newMemberAccess(
            std::unique_ptr<ASTNode> object,
            std::unique_ptr<ASTNode> member);
        static std::unique_ptr<ASTNode> ast_newVector();
        static std::unique_ptr<ASTNode> ast_newIndex(
            std::unique_ptr<ASTNode> object,
            std::unique_ptr<ASTNode> index);
        static std::unique_ptr<ASTNode> ast_clone(const ASTNode& node);
        static std::unique_ptr<ASTNode> ast_newFunctionDeclaration(
            std::unique_ptr<ASTNode> name,
            std::vector<FunctionParameter> parameters,
            std::unique_ptr<ASTNode> body);
        static std::unique_ptr<ASTNode> ast_newMove(
            std::unique_ptr<ASTNode> variable,
            std::unique_ptr<ASTNode> expression
        );
        
        TypeNode getType() const {return type;}
        TypeOp getOp() const {return op;}
        int64_t getInt() const {
            return std::get<int64_t>(as);
        }

        double getFloat() const {
            return std::get<double>(as);
        }

        bool getBool() const {
            return std::get<bool>(as);
        }

        void* getPointer() const {
            return std::get<void*>(as);
        }

        const std::string_view& getString() const {
            return std::get<std::string_view>(as);
        }
            
        void dump(int indent) const;

};

#endif