#ifndef CODEGEN_H
#define CODEGEN_H

#include "tree.h"
#include "semantic.h"
#include <stdio.h>

// 三地址代码操作类型
typedef enum
{
    OP_ASSIGN,      // x = y
    OP_ADD,         // x = y + z
    OP_SUB,         // x = y - z
    OP_MUL,         // x = y * z
    OP_DIV,         // x = y / z
    OP_NEG,         // x = -y
    OP_NOT,         // x = !y
    OP_GT,          // x = y > z
    OP_LT,          // x = y < z
    OP_GE,          // x = y >= z
    OP_LE,          // x = y <= z
    OP_EQ,          // x = y == z
    OP_NE,          // x = y != z
    OP_AND,         // x = y && z
    OP_OR,          // x = y || z
    OP_GOTO,        // goto L
    OP_IF_GOTO,     // if x goto L
    OP_IF_NOT_GOTO, // if !x goto L
    OP_LABEL,       // L:
    OP_CALL,        // x = call f
    OP_ARG,         // arg x (for function calls)
    OP_PARAM,       // param x (for function parameters)
    OP_RETURN,      // return x
    OP_ARRAY_GET,   // x = y[z]
    OP_ARRAY_SET,   // x[y] = z
    OP_ADDR,        // x = &y
    OP_DEREF,       // x = *y
    OP_FUNC_DEF,    // function f:
    OP_FUNC_END     // end function
} OpType;

// 操作数类型
typedef enum
{
    OPERAND_VARIABLE, // 变量
    OPERAND_CONSTANT, // 常量
    OPERAND_TEMP,     // 临时变量
    OPERAND_LABEL,    // 标签
    OPERAND_FUNCTION  // 函数名
} OperandType;

// 操作数结构
typedef struct Operand
{
    OperandType type;
    union
    {
        char *name;        // 变量名、标签名、函数名
        int int_value;     // 整数常量
        float float_value; // 浮点常量
        int temp_no;       // 临时变量编号
    } u;
} Operand;

// 三地址代码指令
typedef struct Instruction
{
    OpType op;                // 操作类型
    Operand *result;          // 结果操作数
    Operand *arg1;            // 第一个操作数
    Operand *arg2;            // 第二个操作数
    struct Instruction *next; // 下一条指令
} Instruction;

// 全局变量
extern Instruction *code_head; // 指令链表头
extern Instruction *code_tail; // 指令链表尾
extern int temp_count;         // 临时变量计数器
extern int label_count;        // 标签计数器

// 函数声明
void init_codegen();
void generate_code(TreeNode *root);

// 操作数操作
Operand *new_operand_variable(const char *name);
Operand *new_operand_constant_int(int value);
Operand *new_operand_constant_float(float value);
Operand *new_operand_temp();
Operand *new_operand_label();
Operand *new_operand_function(const char *name);

// 指令生成
void emit(OpType op, Operand *result, Operand *arg1, Operand *arg2);
Instruction *new_instruction(OpType op, Operand *result, Operand *arg1, Operand *arg2);

// 代码生成函数
Operand *translate_exp(TreeNode *exp);
void translate_stmt(TreeNode *stmt);
void translate_cond(TreeNode *exp, Operand *label_true, Operand *label_false);

// 参数处理函数
int translate_args(TreeNode *args);

// 函数参数列表翻译函数
void translate_varlist(TreeNode *varlist);

// 变量声明翻译函数
void translate_deflist(TreeNode *deflist);
void translate_def(TreeNode *def);
void translate_declist(TreeNode *declist);
void translate_dec(TreeNode *dec);

// 外部定义列表翻译函数
void translate_extdeflist(TreeNode *extdeflist);

// 输出函数
void print_code();
void print_operand(Operand *op);
void save_code_to_file(const char *filename);

// 内存管理
void free_operand(Operand *op);
void free_instruction(Instruction *inst);
void free_all_code();

#endif