#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 语法树节点类型枚举
typedef enum
{
    // 基本类型
    NODE_TYPE,  // int/float
    NODE_ID,    // 标识符
    NODE_INT,   // 整数
    NODE_FLOAT, // 浮点数

    // 符号
    NODE_SEMI,     // ;
    NODE_COMMA,    // ,
    NODE_ASSIGNOP, // =
    NODE_RELOP,    // > < >= <= == !=
    NODE_PLUS,     // +
    NODE_MINUS,    // -
    NODE_STAR,     // *
    NODE_DIV,      // /
    NODE_AND,      // &&
    NODE_OR,       // ||
    NODE_DOT,      // .
    NODE_NOT,      // !
    NODE_LP,       // (
    NODE_RP,       // )
    NODE_LB,       // [
    NODE_RB,       // ]
    NODE_LC,       // {
    NODE_RC,       // }

    // 关键字
    NODE_STRUCT, // struct
    NODE_RETURN, // return
    NODE_IF,     // if
    NODE_ELSE,   // else
    NODE_WHILE,  // while

    // 非终结符
    NODE_PROGRAM,         // Program
    NODE_EXTDEFLIST,      // ExtDefList
    NODE_EXTDEF,          // ExtDef
    NODE_EXTDECLIST,      // ExtDecList
    NODE_SPECIFIER,       // Specifier
    NODE_STRUCTSPECIFIER, // StructSpecifier
    NODE_OPTTAG,          // OptTag
    NODE_TAG,             // Tag
    NODE_VARDEC,          // VarDec
    NODE_FUNDEC,          // FunDec
    NODE_VARLIST,         // VarList
    NODE_PARAMDEC,        // ParamDec
    NODE_COMPST,          // CompSt
    NODE_STMTLIST,        // StmtList
    NODE_STMT,            // Stmt
    NODE_DEFLIST,         // DefList
    NODE_DEF,             // Def
    NODE_DECLIST,         // DecList
    NODE_DEC,             // Dec
    NODE_EXP,             // Exp
    NODE_ARGS             // Args
} NodeType;

// 语法树节点结构
typedef struct TreeNode
{
    NodeType type; // 节点类型
    int lineno;    // 行号
    char *name;    // 节点名称
    union
    {
        int int_value;      // 整数值
        float float_value;  // 浮点数值
        char *string_value; // 字符串值（用于ID等）
    } value;

    struct TreeNode *child;   // 第一个子节点
    struct TreeNode *sibling; // 兄弟节点
} TreeNode;

// 创建新节点
TreeNode *create_node(NodeType type, int lineno, const char *name);

// 添加子节点
void add_child(TreeNode *parent, TreeNode *child);

// 添加兄弟节点
void add_sibling(TreeNode *node, TreeNode *sibling);

// 打印语法树
void print_tree(TreeNode *root, int level);

#endif