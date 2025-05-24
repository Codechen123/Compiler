#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "tree.h"
#include <stdbool.h>

// 类型定义
typedef enum
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRUCT,
    TYPE_ARRAY,
    TYPE_FUNCTION
} DataType;

// 符号类型
typedef enum
{
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_STRUCT
} SymbolType;

// 参数列表结构
typedef struct Param
{
    DataType type;
    struct Param *next;
} Param;

// 符号表项
typedef struct Symbol
{
    char *name;           // 符号名
    SymbolType sym_type;  // 符号类型
    DataType data_type;   // 数据类型
    int lineno;           // 定义行号
    Param *params;        // 函数参数列表（仅函数使用）
    DataType return_type; // 函数返回类型（仅函数使用）
    int array_size;       // 数组大小（仅数组使用）
    struct Symbol *next;  // 下一个符号
} Symbol;

// 符号表结构
typedef struct SymbolTable
{
    Symbol *symbols;
    struct SymbolTable *parent; // 父作用域
} SymbolTable;

// 全局变量
extern SymbolTable *current_scope;
extern int semantic_error_count;

// 函数声明
void init_semantic_analysis();
void semantic_analysis(TreeNode *root);
void analyze_node(TreeNode *node);

// 符号表操作
SymbolTable *create_scope();
void enter_scope();
void exit_scope();
Symbol *lookup_symbol(const char *name);
Symbol *lookup_symbol_current_scope(const char *name);
bool insert_symbol(const char *name, SymbolType sym_type, DataType data_type, int lineno);
bool insert_function(const char *name, DataType return_type, Param *params, int lineno);

// 类型检查
DataType get_exp_type(TreeNode *exp);
bool type_compatible(DataType t1, DataType t2);
Param *create_param(DataType type);
void add_param(Param **head, DataType type);
bool compare_params(Param *p1, Param *p2);
int count_params(Param *params);

// 错误报告
void semantic_error(int type, int lineno, const char *msg);

// 辅助函数char *get_identifier_name(TreeNode *node);DataType get_specifier_type(TreeNode *specifier);void analyze_function_def(TreeNode *extdef);void analyze_variable_def(TreeNode *def);void analyze_expression(TreeNode *exp);void analyze_statement(TreeNode *stmt);void analyze_function_call(TreeNode *exp);void add_params_to_scope(TreeNode *varlist);

#endif