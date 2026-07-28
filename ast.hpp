#ifndef AST
#define AST

#include "lexer.hpp"
#include <memory>

union ASTValue {
            int64_t integer;
            double  real;
            bool    boolean;
            void*   ptr;
            std::string_view text;
            ASTValue() : ptr(nullptr) {};
        };

class ASTNode {

    public:
        enum TypeNode {
            PROGRAM,

            // Literais
            INT,
            FLOAT,
            STRING,
            BOOL,
            null,

            // Identificadores
            VARIABLE,

            // Expressões
            UNARY_EXPR,
            BINARY_EXPR,
            FUNCTION_CALL,

            // Instruções
            ASSIGNMENT,
            RETURN,
            IF,
            WHILE,
            FOR,

            // Declarações
            FUNCTION_DECL,

            OPENPAR, CLOSEPAR,

            INVALID,
        };

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
            NOT,

            // Unários
            POSITIVE,
            NEGATIVE,

            //INVALID,
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

        TypeNode getType() const {return type;}
        TypeOp getOp() const {return op;}
        int64_t getInt() const {return as.integer;}
        double getFloat() const {return as.real;}
        bool getBool() const {return as.boolean;}
        void* getPointer() const {return as.ptr;}
        const std::string_view& getString() const {return as.text;}
            
        void dump(int indent) const;

};

#endif