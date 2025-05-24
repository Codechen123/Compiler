#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 前向声明
void analyze_expression(TreeNode *exp);
void analyze_function_call(TreeNode *exp);
void analyze_statement(TreeNode *stmt);
void analyze_node(TreeNode *node);

// 全局变量
SymbolTable *current_scope = NULL;
int semantic_error_count = 0;
DataType current_function_return_type = TYPE_INT; // 当前函数的返回类型

// 初始化语义分析
void init_semantic_analysis()
{
    current_scope = create_scope();
    semantic_error_count = 0;
}

// 创建新的作用域
SymbolTable *create_scope()
{
    SymbolTable *scope = (SymbolTable *)malloc(sizeof(SymbolTable));
    scope->symbols = NULL;
    scope->parent = NULL;
    return scope;
}

// 进入新作用域
void enter_scope()
{
    SymbolTable *new_scope = create_scope();
    new_scope->parent = current_scope;
    current_scope = new_scope;
}

// 退出当前作用域
void exit_scope()
{
    if (current_scope && current_scope->parent)
    {
        SymbolTable *old_scope = current_scope;
        current_scope = current_scope->parent;
        // 释放旧作用域的内存
        Symbol *sym = old_scope->symbols;
        while (sym)
        {
            Symbol *next = sym->next;
            free(sym->name);
            // 释放参数列表
            Param *param = sym->params;
            while (param)
            {
                Param *next_param = param->next;
                free(param);
                param = next_param;
            }
            free(sym);
            sym = next;
        }
        free(old_scope);
    }
}

// 在当前作用域链中查找符号
Symbol *lookup_symbol(const char *name)
{
    SymbolTable *scope = current_scope;
    while (scope)
    {
        Symbol *sym = scope->symbols;
        while (sym)
        {
            if (strcmp(sym->name, name) == 0)
            {
                return sym;
            }
            sym = sym->next;
        }
        scope = scope->parent;
    }
    return NULL;
}

// 在当前作用域中查找符号
Symbol *lookup_symbol_current_scope(const char *name)
{
    if (!current_scope)
        return NULL;

    Symbol *sym = current_scope->symbols;
    while (sym)
    {
        if (strcmp(sym->name, name) == 0)
        {
            return sym;
        }
        sym = sym->next;
    }
    return NULL;
}

// 插入符号到当前作用域
bool insert_symbol(const char *name, SymbolType sym_type, DataType data_type, int lineno)
{
    // 检查是否在当前作用域中重复定义
    if (lookup_symbol_current_scope(name))
    {
        if (sym_type == SYMBOL_VARIABLE)
        {
            semantic_error(3, lineno, "Redefined variable");
        }
        else if (sym_type == SYMBOL_FUNCTION)
        {
            semantic_error(4, lineno, "Redefined function");
        }
        return false;
    }

    Symbol *sym = (Symbol *)malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->sym_type = sym_type;
    sym->data_type = data_type;
    sym->lineno = lineno;
    sym->params = NULL;
    sym->return_type = data_type;
    sym->array_size = 0;
    sym->next = current_scope->symbols;
    current_scope->symbols = sym;

    return true;
}

// 插入函数符号
bool insert_function(const char *name, DataType return_type, Param *params, int lineno)
{
    // 检查是否在当前作用域中重复定义
    if (lookup_symbol_current_scope(name))
    {
        semantic_error(4, lineno, "Redefined function");
        return false;
    }

    Symbol *sym = (Symbol *)malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->sym_type = SYMBOL_FUNCTION;
    sym->data_type = TYPE_FUNCTION;
    sym->lineno = lineno;
    sym->params = params;
    sym->return_type = return_type;
    sym->array_size = 0;
    sym->next = current_scope->symbols;
    current_scope->symbols = sym;

    return true;
}

// 创建参数
Param *create_param(DataType type)
{
    Param *param = (Param *)malloc(sizeof(Param));
    param->type = type;
    param->next = NULL;
    return param;
}

// 添加参数到参数列表
void add_param(Param **head, DataType type)
{
    Param *new_param = create_param(type);
    if (*head == NULL)
    {
        *head = new_param;
    }
    else
    {
        Param *current = *head;
        while (current->next)
        {
            current = current->next;
        }
        current->next = new_param;
    }
}

// 比较两个参数列表
bool compare_params(Param *p1, Param *p2)
{
    while (p1 && p2)
    {
        if (!type_compatible(p1->type, p2->type))
        {
            return false;
        }
        p1 = p1->next;
        p2 = p2->next;
    }
    return (p1 == NULL && p2 == NULL);
}

// 计算参数个数
int count_params(Param *params)
{
    int count = 0;
    while (params)
    {
        count++;
        params = params->next;
    }
    return count;
}

// 类型兼容性检查
bool type_compatible(DataType t1, DataType t2)
{
    return t1 == t2;
}

// 获取标识符名称
char *get_identifier_name(TreeNode *node)
{
    if (!node)
        return NULL;

    if (node->type == NODE_ID)
    {
        return node->value.string_value;
    }

    // 对于数组类型的VarDec，需要递归查找ID
    if (node->type == NODE_VARDEC && node->child)
    {
        return get_identifier_name(node->child);
    }

    return NULL;
}

// 获取类型说明符的类型
DataType get_specifier_type(TreeNode *specifier)
{
    if (!specifier || !specifier->child)
        return TYPE_INT;

    TreeNode *type_node = specifier->child;
    if (type_node->type == NODE_TYPE)
    {
        if (strcmp(type_node->value.string_value, "int") == 0)
        {
            return TYPE_INT;
        }
        else if (strcmp(type_node->value.string_value, "float") == 0)
        {
            return TYPE_FLOAT;
        }
    }

    return TYPE_INT; // 默认为int类型
}

// 获取表达式的类型
DataType get_exp_type(TreeNode *exp)
{
    if (!exp || !exp->child)
        return TYPE_INT;

    TreeNode *first_child = exp->child;

    switch (first_child->type)
    {
    case NODE_INT:
        return TYPE_INT;
    case NODE_FLOAT:
        return TYPE_FLOAT;
    case NODE_ID:
    {
        // 变量引用
        char *name = first_child->value.string_value;
        Symbol *sym = lookup_symbol(name);
        if (!sym)
        {
            return TYPE_INT;
        }
        return sym->data_type;
    }
    case NODE_LP:
    {
        // 函数调用 (ID LP Args RP) 或括号表达式 (LP Exp RP)
        if (exp->child->sibling && exp->child->sibling->type == NODE_EXP)
        {
            // 括号表达式
            return get_exp_type(exp->child->sibling);
        }
        else
        {
            // 这不应该是函数调用的情况，因为函数调用是 ID LP Args RP
            return TYPE_INT;
        }
    }
    default:
    {
        // 二元运算符
        if (first_child->sibling && first_child->sibling->sibling)
        {
            DataType left_type = get_exp_type(exp->child);
            DataType right_type = get_exp_type(exp->child->sibling->sibling);
            // 简化处理：如果有float则结果为float，否则为int
            if (left_type == TYPE_FLOAT || right_type == TYPE_FLOAT)
            {
                return TYPE_FLOAT;
            }
            return TYPE_INT;
        }
        // 一元运算符
        if (first_child->sibling)
        {
            return get_exp_type(exp->child->sibling);
        }
        return TYPE_INT;
    }
    }
}

// 错误报告
void semantic_error(int type, int lineno, const char *msg)
{
    semantic_error_count++;
    printf("Error type %d at Line %d: %s.\n", type, lineno, msg);
}

// 递归添加参数到当前作用域
void add_params_to_scope(TreeNode *varlist)
{
    if (!varlist || varlist->type != NODE_VARLIST)
        return;

    TreeNode *first_child = varlist->child;
    if (!first_child)
        return;

    // 处理第一个参数
    if (first_child->type == NODE_PARAMDEC)
    {
        DataType param_type = get_specifier_type(first_child->child);
        char *param_name = get_identifier_name(first_child->child->sibling);
        if (param_name)
        {
            insert_symbol(param_name, SYMBOL_VARIABLE, param_type, first_child->lineno);
        }

        // 检查是否有更多参数 (COMMA VarList)
        if (first_child->sibling && first_child->sibling->type == NODE_COMMA &&
            first_child->sibling->sibling && first_child->sibling->sibling->type == NODE_VARLIST)
        {
            // 递归处理剩余的参数
            add_params_to_scope(first_child->sibling->sibling);
        }
    }
}

// 分析表达式
void analyze_expression(TreeNode *exp)
{
    if (!exp)
        return;

    TreeNode *first_child = exp->child;
    if (!first_child)
        return;

    // 函数调用：ID LP Args RP 或 ID LP RP
    if (first_child->type == NODE_ID && first_child->sibling &&
        first_child->sibling->type == NODE_LP)
    {
        analyze_function_call(exp);
        return;
    }

    // 变量引用
    if (first_child->type == NODE_ID && !first_child->sibling)
    {
        char *name = first_child->value.string_value;
        Symbol *sym = lookup_symbol(name);
        if (!sym)
        {
            semantic_error(1, first_child->lineno, "Undefined variable");
        }
        return;
    }

    // 赋值表达式：左边是变量，右边是表达式
    if (first_child->sibling && first_child->sibling->type == NODE_ASSIGNOP)
    {
        // 检查左边的变量是否已定义
        if (first_child->type == NODE_EXP && first_child->child && first_child->child->type == NODE_ID)
        {
            char *name = first_child->child->value.string_value;
            Symbol *sym = lookup_symbol(name);
            if (!sym)
            {
                semantic_error(1, first_child->child->lineno, "Undefined variable");
            }
        }

        // 分析右边的表达式
        TreeNode *right_exp = first_child->sibling->sibling;
        if (right_exp && right_exp->type == NODE_EXP)
        {
            analyze_expression(right_exp);
        }
        return;
    }

    // 递归分析子表达式
    TreeNode *child = exp->child;
    while (child)
    {
        if (child->type == NODE_EXP)
        {
            analyze_expression(child);
        }
        child = child->sibling;
    }
}

// 分析函数调用
void analyze_function_call(TreeNode *exp)
{
    if (!exp || !exp->child)
        return;

    TreeNode *id_node = exp->child;
    char *func_name = id_node->value.string_value;

    // 查找函数
    Symbol *func_sym = lookup_symbol(func_name);
    if (!func_sym || func_sym->sym_type != SYMBOL_FUNCTION)
    {
        semantic_error(2, id_node->lineno, "Undefined function");
        return;
    }

    // 分析实参列表
    Param *actual_params = NULL;
    TreeNode *args = NULL;

    // 找到Args节点
    TreeNode *child = exp->child;
    while (child)
    {
        if (child->type == NODE_ARGS)
        {
            args = child;
            break;
        }
        child = child->sibling;
    }

    // 构建实参类型列表
    if (args)
    {
        TreeNode *arg = args->child;
        while (arg)
        {
            if (arg->type == NODE_EXP)
            {
                DataType arg_type = get_exp_type(arg);
                add_param(&actual_params, arg_type);
                analyze_expression(arg); // 递归分析参数表达式
            }
            arg = arg->sibling;
        }
    }

    // 检查参数匹配
    if (!compare_params(func_sym->params, actual_params))
    {
        semantic_error(6, id_node->lineno, "Function call with wrong argument type");
    }
    else if (count_params(func_sym->params) != count_params(actual_params))
    {
        semantic_error(6, id_node->lineno, "Function call with wrong number of arguments");
    }

    // 释放临时参数列表
    Param *param = actual_params;
    while (param)
    {
        Param *next = param->next;
        free(param);
        param = next;
    }
}

// 分析变量定义
void analyze_variable_def(TreeNode *def)
{
    if (!def || !def->child)
        return;

    TreeNode *specifier = def->child;
    TreeNode *declist = specifier->sibling;

    DataType var_type = get_specifier_type(specifier);

    // 分析声明列表
    TreeNode *dec = declist->child;
    while (dec)
    {
        if (dec->type == NODE_DEC)
        {
            TreeNode *vardec = dec->child;
            char *var_name = get_identifier_name(vardec);
            if (var_name)
            {
                insert_symbol(var_name, SYMBOL_VARIABLE, var_type, vardec->lineno);
            }

            // 如果有初始化表达式，分析它
            if (vardec->sibling && vardec->sibling->sibling)
            {
                analyze_expression(vardec->sibling->sibling);
            }
        }
        dec = dec->sibling;
    }
}

// 分析函数定义
void analyze_function_def(TreeNode *extdef)
{
    if (!extdef || !extdef->child)
        return;

    TreeNode *specifier = extdef->child;
    TreeNode *fundec = specifier->sibling;
    TreeNode *compst = fundec->sibling;

    // 获取返回类型
    DataType return_type = get_specifier_type(specifier);
    current_function_return_type = return_type;

    // 获取函数名
    char *func_name = get_identifier_name(fundec->child);
    if (!func_name)
        return;

    // 分析参数列表
    Param *params = NULL;
    TreeNode *varlist = NULL;

    // 找到参数列表
    if (fundec->child->sibling && fundec->child->sibling->sibling)
    {
        TreeNode *second_child = fundec->child->sibling->sibling;
        if (second_child->type == NODE_VARLIST)
        {
            varlist = second_child;
        }
    }

    // 解析参数
    if (varlist)
    {
        TreeNode *param = varlist->child;
        while (param)
        {
            if (param->type == NODE_PARAMDEC)
            {
                DataType param_type = get_specifier_type(param->child);
                add_param(&params, param_type);
            }
            param = param->sibling;
        }
    }

    // 插入函数到符号表
    insert_function(func_name, return_type, params, fundec->lineno);

    // 进入新作用域分析函数体
    enter_scope();

    // 将参数添加到新作用域中
    if (varlist)
    {
        add_params_to_scope(varlist);
    }

    // 分析函数体
    analyze_node(compst);

    // 退出作用域
    exit_scope();
}

// 分析语句
void analyze_statement(TreeNode *stmt)
{
    if (!stmt || !stmt->child)
        return;

    TreeNode *first_child = stmt->child;

    switch (first_child->type)
    {
    case NODE_RETURN:
    {
        // return语句
        TreeNode *exp = first_child->sibling;
        if (exp && exp->type == NODE_EXP)
        {
            DataType return_type = get_exp_type(exp);
            if (!type_compatible(current_function_return_type, return_type))
            {
                semantic_error(5, first_child->lineno, "Return type mismatch");
            }
            analyze_expression(exp);
        }
        break;
    }
    case NODE_EXP:
    {
        // 表达式语句
        analyze_expression(first_child);
        break;
    }
    case NODE_COMPST:
    {
        // 复合语句
        enter_scope();
        analyze_node(first_child);
        exit_scope();
        break;
    }
    case NODE_IF:
    case NODE_WHILE:
    {
        // 条件语句
        TreeNode *condition = first_child->sibling->sibling; // 跳过LP
        if (condition && condition->type == NODE_EXP)
        {
            analyze_expression(condition);
        }

        // 分析语句体
        TreeNode *body = condition;
        while (body && body->type != NODE_STMT)
        {
            body = body->sibling;
        }
        if (body)
        {
            analyze_statement(body);
        }

        // 如果是if-else，分析else部分
        if (first_child->type == NODE_IF)
        {
            TreeNode *else_part = body;
            while (else_part && else_part->type != NODE_ELSE)
            {
                else_part = else_part->sibling;
            }
            if (else_part && else_part->sibling)
            {
                analyze_statement(else_part->sibling);
            }
        }
        break;
    }
    default:
        break;
    }
}

// 分析节点
void analyze_node(TreeNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case NODE_PROGRAM:
    case NODE_EXTDEFLIST:
    case NODE_STMTLIST:
    case NODE_DEFLIST:
    case NODE_DECLIST:
    {
        // 递归分析子节点
        TreeNode *child = node->child;
        while (child)
        {
            analyze_node(child);
            child = child->sibling;
        }
        break;
    }
    case NODE_EXTDEF:
    {
        // 外部定义：可能是函数定义或变量定义
        TreeNode *specifier = node->child;
        TreeNode *second = specifier ? specifier->sibling : NULL;
        TreeNode *third = second ? second->sibling : NULL;

        if (third && third->type == NODE_COMPST)
        {
            // 函数定义
            analyze_function_def(node);
        }
        else
        {
            // 变量定义
            TreeNode *child = node->child;
            while (child)
            {
                analyze_node(child);
                child = child->sibling;
            }
        }
        break;
    }
    case NODE_DEF:
    {
        // 局部变量定义
        analyze_variable_def(node);
        break;
    }
    case NODE_STMT:
    {
        // 语句
        analyze_statement(node);
        break;
    }
    case NODE_EXP:
    {
        // 表达式
        analyze_expression(node);
        break;
    }
    case NODE_COMPST:
    {
        // 复合语句
        TreeNode *child = node->child;
        while (child)
        {
            analyze_node(child);
            child = child->sibling;
        }
        break;
    }
    default:
    {
        // 其他节点，递归分析子节点
        TreeNode *child = node->child;
        while (child)
        {
            analyze_node(child);
            child = child->sibling;
        }
        break;
    }
    }
}

// 主语义分析函数
void semantic_analysis(TreeNode *root)
{
    if (!root)
        return;

    init_semantic_analysis();
    analyze_node(root);
}