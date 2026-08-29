#ifndef ASTTYPES
#define ASTTYPES

enum TypeNode {
    PROGRAM,

    BLOCK,

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

    // FUNC
    FUNCTION_CALL,
    FUNCTION_DECLARATION,
    FUNCTION_PARAMETERS,
    FUNCTION_PARAMETER,

    // Instruções
    ASSIGNMENT,
    RETURN,
    IF,
    WHILE,
    FOR,
    CONTINUE,
    BREAK,
    MOVE,

    IMPORT,
    MEMBER_ACCESS,

    OPENPAR,
    CLOSEPAR,

    // vetores
    VECTOR,
    INDEX,

    ANYVALUE,

    INVALID,
};

#endif