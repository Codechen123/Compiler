#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 全局变量定义
Instruction *code_head = NULL;
Instruction *code_tail = NULL;
int temp_count = 1;
int label_count = 1;

// 初始化代码生成器
void init_codegen()
{
    code_head = NULL;
    code_tail = NULL;
    temp_count = 1;
    label_count = 1;
}

// 创建变量操作数
Operand *new_operand_variable(const char *name)
{
    Operand *op = (Operand *)malloc(sizeof(Operand));
    op->type = OPERAND_VARIABLE;
    op->u.name = strdup(name);
    return op;
}

// 创建整数常量操作数
Operand *new_operand_constant_int(int value)
{
    Operand *op = (Operand *)malloc(sizeof(Operand));
    op->type = OPERAND_CONSTANT;
    op->u.int_value = value;
    return op;
}

// 创建浮点常量操作数
Operand *new_operand_constant_float(float value)
{
    Operand *op = (Operand *)malloc(sizeof(Operand));
    op->type = OPERAND_CONSTANT_FLOAT;
    op->u.float_value = value;
    return op;
}

// 创建临时变量操作数
Operand *new_operand_temp()
{
    Operand *op = (Operand *)malloc(sizeof(Operand));
    op->type = OPERAND_TEMP;
    op->u.temp_no = temp_count++;
    return op;
}

// 创建标签操作数
Operand *new_operand_label()
{
    Operand *op = (Operand *)malloc(sizeof(Operand));
    op->type = OPERAND_LABEL;
    op->u.temp_no = label_count++;
    return op;
}

// 创建函数名操作数
Operand *new_operand_function(const char *name)
{
    Operand *op = (Operand *)malloc(sizeof(Operand));
    op->type = OPERAND_FUNCTION;
    op->u.name = strdup(name);
    return op;
}

// 创建新指令
Instruction *new_instruction(OpType op, Operand *result, Operand *arg1, Operand *arg2)
{
    Instruction *inst = (Instruction *)malloc(sizeof(Instruction));
    inst->op = op;
    inst->result = result;
    inst->arg1 = arg1;
    inst->arg2 = arg2;
    inst->next = NULL;
    return inst;
}

// 发射一条指令
void emit(OpType op, Operand *result, Operand *arg1, Operand *arg2)
{
    Instruction *inst = new_instruction(op, result, arg1, arg2);

    if (code_head == NULL)
    {
        code_head = code_tail = inst;
    }
    else
    {
        code_tail->next = inst;
        code_tail = inst;
    }
}

// 检查并处理结构体成员访问
Operand *handle_struct_member_access(TreeNode *exp)
{
    if (exp == NULL || exp->type != NODE_EXP)
        return NULL;

    TreeNode *child = exp->child;
    if (child == NULL)
        return NULL;

    // 检查是否是结构体成员访问：ID DOT ID 或 EXP DOT ID
    if (child->sibling != NULL && child->sibling->type == NODE_DOT &&
        child->sibling->sibling != NULL && child->sibling->sibling->type == NODE_ID)
    {
        TreeNode *struct_node = child;
        TreeNode *member_node = child->sibling->sibling;

        // 如果第一个子节点是表达式，需要进一步解析
        if (struct_node->type == NODE_EXP && struct_node->child != NULL &&
            struct_node->child->type == NODE_ID)
        {
            struct_node = struct_node->child;
        }

        if (struct_node->type == NODE_ID)
        {
            char *struct_name = struct_node->value.string_value;
            char *member_name = member_node->value.string_value;

            // 创建组合变量名
            int len = strlen(struct_name) + strlen(member_name) + 2;
            char *combined_name = (char *)malloc(len);
            snprintf(combined_name, len, "%s_%s", struct_name, member_name);

            Operand *member_var = new_operand_variable(combined_name);
            free(combined_name);
            return member_var;
        }
    }

    // 如果当前层不是结构体成员访问，递归检查子表达式
    if (child->type == NODE_EXP)
    {
        return handle_struct_member_access(child);
    }

    return NULL;
}

// 检查表达式是否是数组访问，如果是则返回数组名和索引
bool is_array_access(TreeNode *exp, TreeNode **array_node, TreeNode **index_node)
{
    if (exp == NULL || exp->type != NODE_EXP)
        return false;

    TreeNode *child = exp->child;
    if (child == NULL)
        return false;

    // 检查是否是数组访问：EXP LB EXP RB
    if (child->sibling != NULL && child->sibling->type == NODE_LB &&
        child->sibling->sibling != NULL && child->sibling->sibling->type == NODE_EXP &&
        child->sibling->sibling->sibling != NULL && child->sibling->sibling->sibling->type == NODE_RB)
    {
        *array_node = child;
        *index_node = child->sibling->sibling;
        return true;
    }

    return false;
}

// 翻译表达式
Operand *translate_exp(TreeNode *exp)
{
    if (exp == NULL)
        return NULL;

    switch (exp->type)
    {
    case NODE_INT:
    {
        return new_operand_constant_int(exp->value.int_value);
    }

    case NODE_FLOAT:
    {
        return new_operand_constant_float(exp->value.float_value);
    }

    case NODE_ID:
    {
        return new_operand_variable(exp->value.string_value);
    }

    case NODE_EXP:
    {
        TreeNode *child = exp->child;
        if (child == NULL)
            return NULL;

        // 首先检查是否是直接的结构体成员访问（不是赋值表达式）
        if (child->sibling == NULL || child->sibling->type != NODE_ASSIGNOP)
        {
            Operand *struct_member = handle_struct_member_access(exp);
            if (struct_member != NULL)
            {
                return struct_member;
            }
        }

        // 处理括号表达式：LP Exp RP
        if (child->type == NODE_LP && child->sibling != NULL && child->sibling->sibling != NULL && child->sibling->sibling->type == NODE_RP)
        {
            return translate_exp(child->sibling);
        }

        // 处理函数调用（必须首先检查具体的函数调用模式）
        if (child->type == NODE_ID && child->sibling != NULL && child->sibling->type == NODE_LP)
        {
            TreeNode *func_name = child;
            TreeNode *lp = child->sibling;
            TreeNode *args = lp->sibling;
            TreeNode *rp = args ? args->sibling : NULL;

            // 验证完整的函数调用模式：ID LP Args RP 或 ID LP RP
            if ((args && args->type == NODE_ARGS && rp && rp->type == NODE_RP) ||
                (args && args->type == NODE_RP)) // 无参数函数调用的情况
            {
                // 处理参数
                if (args && args->type == NODE_ARGS)
                {
                    translate_args(args);
                }

                // 生成函数调用
                Operand *result = new_operand_temp();
                Operand *func = new_operand_function(func_name->value.string_value);
                emit(OP_CALL, result, func, NULL);
                return result;
            }
        }

        // 处理双目运算
        if (child->sibling != NULL && child->sibling->sibling != NULL)
        {
            TreeNode *left = child;
            TreeNode *op = child->sibling;
            TreeNode *right = child->sibling->sibling;

            // 排除函数调用模式
            if (!(child->type == NODE_ID && child->sibling->type == NODE_LP))
            {
                switch (op->type)
                {
                case NODE_ASSIGNOP:
                {
                    // 检查左操作数是否是结构体成员访问
                    Operand *struct_member = handle_struct_member_access(left);
                    if (struct_member != NULL)
                    {
                        // 这是结构体成员访问的赋值
                        Operand *value = translate_exp(right);
                        emit(OP_ASSIGN, struct_member, value, NULL);
                        return struct_member;
                    }

                    // 检查左操作数是否是数组访问
                    TreeNode *array_node = NULL;
                    TreeNode *index_node = NULL;
                    if (is_array_access(left, &array_node, &index_node))
                    {
                        // 这是数组赋值：arr[index] = value
                        Operand *array_name = translate_exp(array_node);
                        Operand *index = translate_exp(index_node);
                        Operand *value = translate_exp(right);

                        // 生成数组赋值指令：arr[index] := value
                        emit(OP_ARRAY_SET, array_name, index, value);
                        return array_name;
                    }

                    // 普通赋值
                    Operand *t1 = translate_exp(left);
                    Operand *t2 = translate_exp(right);
                    emit(OP_ASSIGN, t1, t2, NULL);
                    return t1;
                }
                default:
                {
                    Operand *t1 = translate_exp(left);
                    Operand *t2 = translate_exp(right);

                    switch (op->type)
                    {
                    case NODE_PLUS:
                    case NODE_MINUS:
                    case NODE_STAR:
                    case NODE_DIV:
                    case NODE_RELOP:
                    case NODE_AND:
                    case NODE_OR:
                    case NODE_LB:  // 数组访问 exp[exp]
                    case NODE_DOT: // 结构体成员访问 exp.id
                    {
                        Operand *result = new_operand_temp();

                        switch (op->type)
                        {
                        case NODE_PLUS:
                            emit(OP_ADD, result, t1, t2);
                            break;
                        case NODE_MINUS:
                            emit(OP_SUB, result, t1, t2);
                            break;
                        case NODE_STAR:
                            emit(OP_MUL, result, t1, t2);
                            break;
                        case NODE_DIV:
                            emit(OP_DIV, result, t1, t2);
                            break;
                        case NODE_RELOP:
                        {
                            if (strcmp(op->name, ">") == 0)
                            {
                                emit(OP_GT, result, t1, t2);
                            }
                            else if (strcmp(op->name, "<") == 0)
                            {
                                emit(OP_LT, result, t1, t2);
                            }
                            else if (strcmp(op->name, ">=") == 0)
                            {
                                emit(OP_GE, result, t1, t2);
                            }
                            else if (strcmp(op->name, "<=") == 0)
                            {
                                emit(OP_LE, result, t1, t2);
                            }
                            else if (strcmp(op->name, "==") == 0)
                            {
                                emit(OP_EQ, result, t1, t2);
                            }
                            else if (strcmp(op->name, "!=") == 0)
                            {
                                emit(OP_NE, result, t1, t2);
                            }
                            break;
                        }
                        case NODE_AND:
                            emit(OP_AND, result, t1, t2);
                            break;
                        case NODE_OR:
                            emit(OP_OR, result, t1, t2);
                            break;
                        case NODE_LB: // 数组访问 exp[exp]
                            emit(OP_ARRAY_GET, result, t1, t2);
                            break;
                        case NODE_DOT: // 结构体成员访问 exp.id
                        {
                            // 对于结构体成员访问，我们生成一个组合的变量名
                            // 例如：p.x -> p_x
                            if (left->type == NODE_ID && right->type == NODE_ID)
                            {
                                char *struct_name = left->value.string_value;
                                char *member_name = right->value.string_value;

                                // 创建组合变量名
                                int len = strlen(struct_name) + strlen(member_name) + 2; // +2 for '_' and '\0'
                                char *combined_name = (char *)malloc(len);
                                snprintf(combined_name, len, "%s_%s", struct_name, member_name);

                                // 创建变量操作数
                                Operand *member_var = new_operand_variable(combined_name);
                                free(combined_name);
                                return member_var;
                            }
                            else
                            {
                                // 如果不是简单的ID.ID形式，返回临时变量
                                return result;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                        return result;
                    }
                    default:
                        break;
                    }
                    break;
                }
                }
            }
        }

        // 处理单目运算
        if (child->sibling != NULL)
        {
            TreeNode *op = child;
            TreeNode *operand = child->sibling;

            Operand *t1 = translate_exp(operand);
            Operand *result = new_operand_temp();

            switch (op->type)
            {
            case NODE_MINUS:
                emit(OP_NEG, result, t1, NULL);
                break;
            case NODE_NOT:
                emit(OP_NOT, result, t1, NULL);
                break;
            default:
                break;
            }
            return result;
        }

        // 递归处理子表达式
        return translate_exp(child);
    }

    default:
        return NULL;
    }

    return NULL;
}

// 翻译条件表达式（用于if和while语句）
void translate_cond(TreeNode *exp, Operand *label_true, Operand *label_false)
{
    if (exp == NULL)
        return;

    if (exp->type == NODE_EXP)
    {
        TreeNode *child = exp->child;
        if (child != NULL && child->sibling != NULL && child->sibling->sibling != NULL)
        {
            TreeNode *left = child;
            TreeNode *op = child->sibling;
            TreeNode *right = child->sibling->sibling;

            // 处理逻辑运算符
            if (op->type == NODE_AND)
            {
                Operand *label1 = new_operand_label();
                translate_cond(left, label1, label_false);
                emit(OP_LABEL, label1, NULL, NULL);
                translate_cond(right, label_true, label_false);
                return;
            }
            else if (op->type == NODE_OR)
            {
                Operand *label1 = new_operand_label();
                translate_cond(left, label_true, label1);
                emit(OP_LABEL, label1, NULL, NULL);
                translate_cond(right, label_true, label_false);
                return;
            }
            // 处理关系运算符
            else if (op->type == NODE_RELOP)
            {
                Operand *t1 = translate_exp(left);
                Operand *t2 = translate_exp(right);
                Operand *temp_result = new_operand_temp();

                if (strcmp(op->name, ">") == 0)
                {
                    emit(OP_GT, temp_result, t1, t2);
                }
                else if (strcmp(op->name, "<") == 0)
                {
                    emit(OP_LT, temp_result, t1, t2);
                }
                else if (strcmp(op->name, ">=") == 0)
                {
                    emit(OP_GE, temp_result, t1, t2);
                }
                else if (strcmp(op->name, "<=") == 0)
                {
                    emit(OP_LE, temp_result, t1, t2);
                }
                else if (strcmp(op->name, "==") == 0)
                {
                    emit(OP_EQ, temp_result, t1, t2);
                }
                else if (strcmp(op->name, "!=") == 0)
                {
                    emit(OP_NE, temp_result, t1, t2);
                }

                emit(OP_IF_GOTO, NULL, temp_result, label_true);
                emit(OP_GOTO, NULL, label_false, NULL);
                return;
            }
        }
    }

    // 普通条件表达式
    Operand *t1 = translate_exp(exp);
    emit(OP_IF_GOTO, NULL, t1, label_true);
    emit(OP_GOTO, NULL, label_false, NULL);
}

// 翻译变量声明列表
void translate_deflist(TreeNode *deflist)
{
    if (deflist == NULL)
        return;

    if (deflist->type == NODE_DEFLIST)
    {
        TreeNode *child = deflist->child;
        while (child != NULL)
        {
            if (child->type == NODE_DEF)
            {
                translate_def(child);
            }
            else if (child->type == NODE_DEFLIST)
            {
                translate_deflist(child);
            }
            child = child->sibling;
        }
    }
}

// 翻译变量定义
void translate_def(TreeNode *def)
{
    if (def == NULL || def->type != NODE_DEF)
        return;

    TreeNode *child = def->child;
    while (child != NULL)
    {
        if (child->type == NODE_DECLIST)
        {
            translate_declist(child);
        }
        child = child->sibling;
    }
}

// 翻译声明列表
void translate_declist(TreeNode *declist)
{
    if (declist == NULL || declist->type != NODE_DECLIST)
        return;

    TreeNode *child = declist->child;
    while (child != NULL)
    {
        if (child->type == NODE_DEC)
        {
            translate_dec(child);
        }
        else if (child->type == NODE_DECLIST)
        {
            translate_declist(child);
        }
        child = child->sibling;
    }
}

// 翻译单个声明
void translate_dec(TreeNode *dec)
{
    if (dec == NULL || dec->type != NODE_DEC)
        return;

    TreeNode *child = dec->child;
    if (child == NULL)
        return;

    // 检查是否有初始化: VarDec ASSIGNOP Exp
    if (child->sibling != NULL && child->sibling->type == NODE_ASSIGNOP)
    {
        TreeNode *var_dec = child;
        TreeNode *exp = child->sibling->sibling;

        // 获取变量名 (从VarDec中提取)
        TreeNode *var_id = var_dec->child;
        if (var_id != NULL && var_id->type == NODE_ID)
        {
            // 翻译初始化表达式
            Operand *init_value = translate_exp(exp);
            Operand *var = new_operand_variable(var_id->value.string_value);

            // 生成赋值指令
            emit(OP_ASSIGN, var, init_value, NULL);
        }
    }
    // 如果只是声明没有初始化，不需要生成代码
}

// 翻译语句
void translate_stmt(TreeNode *stmt)
{
    if (stmt == NULL)
        return;

    switch (stmt->type)
    {
    case NODE_STMT:
    {
        TreeNode *child = stmt->child;
        if (child == NULL)
            return;

        switch (child->type)
        {
        case NODE_EXP:
        {
            // 表达式语句
            translate_exp(child);
            break;
        }

        case NODE_COMPST:
        {
            // 复合语句
            translate_stmt(child);
            break;
        }

        case NODE_RETURN:
        {
            // return语句
            TreeNode *exp = child->sibling;
            if (exp != NULL)
            {
                Operand *t1 = translate_exp(exp);
                emit(OP_RETURN, t1, NULL, NULL);
            }
            else
            {
                emit(OP_RETURN, NULL, NULL, NULL);
            }
            break;
        }

        case NODE_IF:
        {
            // if语句
            TreeNode *exp = child->sibling->sibling; // LP Exp RP
            TreeNode *stmt1 = exp->sibling->sibling;
            TreeNode *stmt2 = NULL;

            // 检查是否有else子句
            if (stmt1->sibling != NULL && stmt1->sibling->type == NODE_ELSE)
            {
                stmt2 = stmt1->sibling->sibling;
            }

            Operand *label1 = new_operand_label();
            Operand *label2 = new_operand_label();

            if (stmt2 != NULL)
            {
                // if-else语句
                translate_cond(exp, label1, label2);
                emit(OP_LABEL, label1, NULL, NULL);
                translate_stmt(stmt1);
                Operand *label3 = new_operand_label();
                emit(OP_GOTO, NULL, label3, NULL);
                emit(OP_LABEL, label2, NULL, NULL);
                translate_stmt(stmt2);
                emit(OP_LABEL, label3, NULL, NULL);
            }
            else
            {
                // if语句
                translate_cond(exp, label1, label2);
                emit(OP_LABEL, label1, NULL, NULL);
                translate_stmt(stmt1);
                emit(OP_LABEL, label2, NULL, NULL);
            }
            break;
        }

        case NODE_WHILE:
        {
            // while语句
            TreeNode *exp = child->sibling->sibling; // LP Exp RP
            TreeNode *stmt_body = exp->sibling->sibling;

            Operand *label1 = new_operand_label();
            Operand *label2 = new_operand_label();
            Operand *label3 = new_operand_label();

            emit(OP_LABEL, label1, NULL, NULL);
            translate_cond(exp, label2, label3);
            emit(OP_LABEL, label2, NULL, NULL);
            translate_stmt(stmt_body);
            emit(OP_GOTO, NULL, label1, NULL);
            emit(OP_LABEL, label3, NULL, NULL);
            break;
        }

        default:
            break;
        }
        break;
    }

    case NODE_COMPST:
    {
        // 复合语句：CompSt -> LC DefList StmtList RC | LC StmtList RC
        TreeNode *current = stmt->child->sibling; // 跳过LC

        // 检查是否有DefList
        if (current != NULL && current->type == NODE_DEFLIST)
        {
            translate_deflist(current); // 翻译DefList
            current = current->sibling; // 继续处理后续节点
        }

        // 处理StmtList
        if (current != NULL && current->type == NODE_STMTLIST)
        {
            translate_stmt(current);
        }
        break;
    }

    case NODE_STMTLIST:
    {
        // 语句列表：StmtList -> Stmt StmtList | ε
        TreeNode *child = stmt->child;
        while (child != NULL)
        {
            if (child->type == NODE_STMT)
            {
                translate_stmt(child);
            }
            else if (child->type == NODE_STMTLIST)
            {
                translate_stmt(child);
            }
            child = child->sibling;
        }
        break;
    }

    default:
        break;
    }
}

// 翻译函数参数列表
void translate_varlist(TreeNode *varlist)
{
    if (varlist == NULL || varlist->type != NODE_VARLIST)
        return;

    TreeNode *current = varlist;
    while (current != NULL && current->type == NODE_VARLIST)
    {
        TreeNode *paramdec = current->child;
        if (paramdec != NULL && paramdec->type == NODE_PARAMDEC)
        {
            // ParamDec -> Specifier VarDec
            TreeNode *vardec = paramdec->child->sibling; // 跳过Specifier
            if (vardec != NULL && vardec->type == NODE_VARDEC)
            {
                // VarDec -> ID (简单情况)
                TreeNode *param_id = vardec->child;
                if (param_id != NULL && param_id->type == NODE_ID)
                {
                    Operand *param = new_operand_variable(param_id->value.string_value);
                    emit(OP_PARAM, param, NULL, NULL);
                }
            }
        }

        // 查找下一个参数：ParamDec -> COMMA -> VarList
        TreeNode *comma = paramdec ? paramdec->sibling : NULL;
        if (comma != NULL && comma->type == NODE_COMMA)
        {
            current = comma->sibling; // 下一个VarList节点
        }
        else
        {
            break; // 没有更多参数
        }
    }
}

// 翻译函数定义
void translate_function_def(TreeNode *extdef)
{
    if (extdef == NULL)
        return;

    TreeNode *specifier = extdef->child;
    TreeNode *fundec = specifier->sibling;
    TreeNode *compst = fundec->sibling;

    // 获取函数名
    TreeNode *func_name = fundec->child;
    if (func_name != NULL && func_name->type == NODE_ID)
    {
        Operand *func = new_operand_function(func_name->value.string_value);
        emit(OP_FUNC_DEF, func, NULL, NULL);

        // 处理函数参数：FunDec -> ID LP VarList RP 或 ID LP RP
        TreeNode *lp = func_name->sibling;
        if (lp != NULL && lp->type == NODE_LP)
        {
            TreeNode *next = lp->sibling;
            if (next != NULL && next->type == NODE_VARLIST)
            {
                // 有参数列表
                translate_varlist(next);
            }
            // 如果next是RP，说明无参数，不需要处理
        }

        // 翻译函数体
        translate_stmt(compst);

        emit(OP_FUNC_END, func, NULL, NULL);
    }
}

// 递归处理外部定义列表
void translate_extdeflist(TreeNode *extdeflist)
{
    if (extdeflist == NULL || extdeflist->type != NODE_EXTDEFLIST)
        return;

    TreeNode *extdef = extdeflist->child;
    if (extdef != NULL && extdef->type == NODE_EXTDEF)
    {
        TreeNode *specifier = extdef->child;
        TreeNode *second = specifier->sibling;

        if (second != NULL && second->type == NODE_FUNDEC)
        {
            // 函数定义
            translate_function_def(extdef);
        }
        // 变量定义不需要生成代码

        // 处理下一个ExtDefList（通过sibling连接）
        TreeNode *next_extdeflist = extdef->sibling;
        if (next_extdeflist != NULL && next_extdeflist->type == NODE_EXTDEFLIST)
        {
            translate_extdeflist(next_extdeflist);
        }
    }
}

// 主代码生成入口
void generate_code(TreeNode *root)
{
    if (root == NULL)
        return;

    init_codegen();

    // 遍历程序的外部定义列表
    if (root->type == NODE_PROGRAM)
    {
        TreeNode *extdeflist = root->child;
        if (extdeflist != NULL && extdeflist->type == NODE_EXTDEFLIST)
        {
            translate_extdeflist(extdeflist);
        }
    }
}

// 打印操作数
void print_operand(Operand *op)
{
    if (op == NULL)
    {
        printf("NULL");
        return;
    }

    switch (op->type)
    {
    case OPERAND_VARIABLE:
        printf("%s", op->u.name);
        break;
    case OPERAND_CONSTANT:
        printf("#%d", op->u.int_value);
        break;
    case OPERAND_CONSTANT_FLOAT:
        printf("#%.5f", op->u.float_value);
        break;
    case OPERAND_TEMP:
        printf("t%d", op->u.temp_no);
        break;
    case OPERAND_LABEL:
        printf("label%d", op->u.temp_no);
        break;
    case OPERAND_FUNCTION:
        printf("%s", op->u.name);
        break;
    }
}

// 打印三地址代码
void print_code()
{
    Instruction *inst = code_head;
    while (inst != NULL)
    {
        // 跳过被标记为死代码的指令（通过检查是否是被错误标记的OP_LABEL）
        if (inst->op == OP_LABEL && inst->result &&
            (inst->result->type == OPERAND_VARIABLE || inst->result->type == OPERAND_TEMP))
        {
            // 这是被标记为死代码的赋值指令，跳过打印
            inst = inst->next;
            continue;
        }

        switch (inst->op)
        {
        case OP_ASSIGN:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            break;
        case OP_ADD:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" + ");
            print_operand(inst->arg2);
            break;
        case OP_SUB:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" - ");
            print_operand(inst->arg2);
            break;
        case OP_MUL:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" * ");
            print_operand(inst->arg2);
            break;
        case OP_DIV:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" / ");
            print_operand(inst->arg2);
            break;
        case OP_NEG:
            print_operand(inst->result);
            printf(" := -");
            print_operand(inst->arg1);
            break;
        case OP_NOT:
            print_operand(inst->result);
            printf(" := !");
            print_operand(inst->arg1);
            break;
        case OP_GT:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" > ");
            print_operand(inst->arg2);
            break;
        case OP_LT:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" < ");
            print_operand(inst->arg2);
            break;
        case OP_GE:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" >= ");
            print_operand(inst->arg2);
            break;
        case OP_LE:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" <= ");
            print_operand(inst->arg2);
            break;
        case OP_EQ:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" == ");
            print_operand(inst->arg2);
            break;
        case OP_NE:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" != ");
            print_operand(inst->arg2);
            break;
        case OP_AND:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" && ");
            print_operand(inst->arg2);
            break;
        case OP_OR:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf(" || ");
            print_operand(inst->arg2);
            break;
        case OP_GOTO:
            printf("GOTO ");
            print_operand(inst->arg1);
            break;
        case OP_IF_GOTO:
            printf("IF ");
            print_operand(inst->arg1);
            printf(" GOTO ");
            print_operand(inst->arg2);
            break;
        case OP_IF_NOT_GOTO:
            printf("IF NOT ");
            print_operand(inst->arg1);
            printf(" GOTO ");
            print_operand(inst->arg2);
            break;
        case OP_LABEL:
            print_operand(inst->result);
            printf(" :");
            break;
        case OP_CALL:
            print_operand(inst->result);
            printf(" := CALL ");
            print_operand(inst->arg1);
            break;
        case OP_ARG:
            printf("ARG ");
            print_operand(inst->result);
            break;
        case OP_PARAM:
            printf("PARAM ");
            print_operand(inst->result);
            break;
        case OP_RETURN:
            printf("RETURN ");
            if (inst->result != NULL)
            {
                print_operand(inst->result);
            }
            break;
        case OP_ARRAY_GET:
            print_operand(inst->result);
            printf(" := ");
            print_operand(inst->arg1);
            printf("[");
            print_operand(inst->arg2);
            printf("]");
            break;
        case OP_ARRAY_SET:
            print_operand(inst->result);
            printf("[");
            print_operand(inst->arg1);
            printf("] := ");
            print_operand(inst->arg2);
            break;
        case OP_FUNC_DEF:
            printf("FUNCTION ");
            print_operand(inst->result);
            printf(" :");
            break;
        case OP_FUNC_END:
            printf("END FUNCTION ");
            print_operand(inst->result);
            break;
        default:
            break;
        }
        printf("\n");
        inst = inst->next;
    }
}

// 辅助函数：将操作数输出到文件
static void fprint_operand(FILE *file, Operand *op)
{
    if (op == NULL)
    {
        fprintf(file, "NULL");
        return;
    }

    switch (op->type)
    {
    case OPERAND_VARIABLE:
        fprintf(file, "%s", op->u.name);
        break;
    case OPERAND_CONSTANT:
        fprintf(file, "#%d", op->u.int_value);
        break;
    case OPERAND_CONSTANT_FLOAT:
        fprintf(file, "#%.5f", op->u.float_value);
        break;
    case OPERAND_TEMP:
        fprintf(file, "t%d", op->u.temp_no);
        break;
    case OPERAND_LABEL:
        fprintf(file, "label%d", op->u.temp_no);
        break;
    case OPERAND_FUNCTION:
        fprintf(file, "%s", op->u.name);
        break;
    }
}

// 保存代码到文件
void save_code_to_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Cannot open file %s for writing\n", filename);
        return;
    }

    Instruction *inst = code_head;
    while (inst != NULL)
    {
        // 跳过被标记为死代码的指令（通过检查是否是被错误标记的OP_LABEL）
        if (inst->op == OP_LABEL && inst->result &&
            (inst->result->type == OPERAND_VARIABLE || inst->result->type == OPERAND_TEMP))
        {
            // 这是被标记为死代码的赋值指令，跳过保存
            inst = inst->next;
            continue;
        }

        switch (inst->op)
        {
        case OP_ASSIGN:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            break;
        case OP_ADD:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " + ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_SUB:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " - ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_MUL:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " * ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_DIV:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " / ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_NEG:
            fprint_operand(file, inst->result);
            fprintf(file, " := -");
            fprint_operand(file, inst->arg1);
            break;
        case OP_NOT:
            fprint_operand(file, inst->result);
            fprintf(file, " := !");
            fprint_operand(file, inst->arg1);
            break;
        case OP_GT:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " > ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_LT:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " < ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_GE:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " >= ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_LE:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " <= ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_EQ:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " == ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_NE:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " != ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_AND:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " && ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_OR:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " || ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_GOTO:
            fprintf(file, "GOTO ");
            fprint_operand(file, inst->arg1);
            break;
        case OP_IF_GOTO:
            fprintf(file, "IF ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " GOTO ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_IF_NOT_GOTO:
            fprintf(file, "IF NOT ");
            fprint_operand(file, inst->arg1);
            fprintf(file, " GOTO ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_LABEL:
            fprint_operand(file, inst->result);
            fprintf(file, " :");
            break;
        case OP_CALL:
            fprint_operand(file, inst->result);
            fprintf(file, " := CALL ");
            fprint_operand(file, inst->arg1);
            break;
        case OP_ARG:
            fprintf(file, "ARG ");
            fprint_operand(file, inst->result);
            break;
        case OP_PARAM:
            fprintf(file, "PARAM ");
            fprint_operand(file, inst->result);
            break;
        case OP_RETURN:
            fprintf(file, "RETURN ");
            if (inst->result != NULL)
            {
                fprint_operand(file, inst->result);
            }
            break;
        case OP_ARRAY_GET:
            fprint_operand(file, inst->result);
            fprintf(file, " := ");
            fprint_operand(file, inst->arg1);
            fprintf(file, "[");
            fprint_operand(file, inst->arg2);
            fprintf(file, "]");
            break;
        case OP_ARRAY_SET:
            fprint_operand(file, inst->result);
            fprintf(file, "[");
            fprint_operand(file, inst->arg1);
            fprintf(file, "] := ");
            fprint_operand(file, inst->arg2);
            break;
        case OP_FUNC_DEF:
            fprintf(file, "FUNCTION ");
            fprint_operand(file, inst->result);
            fprintf(file, " :");
            break;
        case OP_FUNC_END:
            fprintf(file, "END FUNCTION ");
            fprint_operand(file, inst->result);
            break;
        default:
            break;
        }
        fprintf(file, "\n");
        inst = inst->next;
    }

    fclose(file);
}

// 内存管理函数
void free_operand(Operand *op)
{
    if (op == NULL)
        return;

    if (op->type == OPERAND_VARIABLE || op->type == OPERAND_FUNCTION)
    {
        free(op->u.name);
    }
    free(op);
}

void free_instruction(Instruction *inst)
{
    if (inst == NULL)
        return;

    free_operand(inst->result);
    free_operand(inst->arg1);
    free_operand(inst->arg2);
    free(inst);
}

void free_all_code()
{
    Instruction *inst = code_head;
    while (inst != NULL)
    {
        Instruction *next = inst->next;
        free_instruction(inst);
        inst = next;
    }
    code_head = code_tail = NULL;
}

// 翻译参数列表
int translate_args(TreeNode *args)
{
    if (args == NULL || args->type != NODE_ARGS)
        return 0;

    int param_count = 0;
    TreeNode *current = args;

    while (current != NULL && current->type == NODE_ARGS)
    {
        TreeNode *exp = current->child;
        if (exp != NULL && exp->type == NODE_EXP)
        {
            // 翻译参数表达式
            Operand *param = translate_exp(exp);
            emit(OP_ARG, param, NULL, NULL);
            param_count++;

            // 查找下一个参数：Exp -> COMMA -> Args
            TreeNode *comma = exp->sibling;
            if (comma != NULL && comma->type == NODE_COMMA)
            {
                current = comma->sibling; // 下一个Args节点
            }
            else
            {
                break; // 没有更多参数
            }
        }
        else
        {
            break;
        }
    }

    return param_count;
}

// ========================= 中间代码优化实现 =========================

// 优化统计全局变量
OptimizationStats opt_stats;

// 初始化优化统计
void init_optimization_stats()
{
    opt_stats.constant_folding_count = 0;
    opt_stats.constant_propagation_count = 0;
    opt_stats.dead_code_elimination_count = 0;
    opt_stats.common_subexpression_count = 0;
    opt_stats.redundant_assignment_count = 0;
    opt_stats.array_access_optimization_count = 0;
    opt_stats.total_instructions_before = 0;
    opt_stats.total_instructions_after = 0;
}

// 统计指令数量
int count_instructions()
{
    int count = 0;
    Instruction *inst = code_head;
    while (inst != NULL)
    {
        // 跳过被标记为死代码的指令
        if (!(inst->op == OP_LABEL && inst->result &&
              (inst->result->type == OPERAND_VARIABLE || inst->result->type == OPERAND_TEMP)))
        {
            count++;
        }
        inst = inst->next;
    }
    return count;
}

// 打印优化统计信息
void print_optimization_stats()
{
    printf("\n=== Code Optimization Statistics ===\n");
    printf("Instructions before optimization: %d\n", opt_stats.total_instructions_before);
    printf("Instructions after optimization:  %d\n", opt_stats.total_instructions_after);
    printf("Instructions eliminated: %d (%.1f%%)\n",
           opt_stats.total_instructions_before - opt_stats.total_instructions_after,
           opt_stats.total_instructions_before > 0 ? 100.0 * (opt_stats.total_instructions_before - opt_stats.total_instructions_after) / opt_stats.total_instructions_before : 0.0);
    printf("\nOptimization breakdown:\n");
    printf("- Constant folding:           %d\n", opt_stats.constant_folding_count);
    printf("- Constant propagation:       %d\n", opt_stats.constant_propagation_count);
    printf("- Dead code elimination:      %d\n", opt_stats.dead_code_elimination_count);
    printf("- Common subexpression:       %d\n", opt_stats.common_subexpression_count);
    printf("- Redundant assignment:       %d\n", opt_stats.redundant_assignment_count);
    printf("- Array access optimization:  %d\n", opt_stats.array_access_optimization_count);
    printf("=====================================\n\n");
}

// 检查操作数是否为常量
bool is_constant_operand(Operand *op)
{
    return op != NULL && (op->type == OPERAND_CONSTANT || op->type == OPERAND_CONSTANT_FLOAT);
}

// 获取常量值（仅适用于整数常量）
int get_constant_value(Operand *op)
{
    if (op != NULL && op->type == OPERAND_CONSTANT)
        return op->u.int_value;
    return 0;
}

// 检查两个操作数是否相等
bool operands_equal(Operand *op1, Operand *op2)
{
    if (op1 == NULL || op2 == NULL)
        return false;

    if (op1->type != op2->type)
        return false;

    switch (op1->type)
    {
    case OPERAND_VARIABLE:
    case OPERAND_FUNCTION:
        return strcmp(op1->u.name, op2->u.name) == 0;
    case OPERAND_CONSTANT:
        return op1->u.int_value == op2->u.int_value;
    case OPERAND_CONSTANT_FLOAT:
        return op1->u.float_value == op2->u.float_value;
    case OPERAND_TEMP:
    case OPERAND_LABEL:
        return op1->u.temp_no == op2->u.temp_no;
    }
    return false;
}

// 检查指令是否为死代码
bool is_dead_instruction(Instruction *inst)
{
    if (inst == NULL)
        return false;

    // 有副作用的指令不能删除
    switch (inst->op)
    {
    case OP_CALL:
    case OP_ARG:
    case OP_PARAM:
    case OP_RETURN:
    case OP_ARRAY_SET:
    case OP_GOTO:
    case OP_IF_GOTO:
    case OP_IF_NOT_GOTO:
    case OP_LABEL:
    case OP_FUNC_DEF:
    case OP_FUNC_END:
        return false;
    default:
        break;
    }

    // 检查结果是否被使用
    if (inst->result == NULL)
        return false;

    Instruction *current = inst->next;
    int search_limit = 1000; // 防止无限循环，最多搜索1000条指令
    int search_count = 0;

    while (current != NULL && search_count < search_limit)
    {
        search_count++;

        // 检查当前指令是否使用了inst的结果
        if ((current->arg1 && operands_equal(current->arg1, inst->result)) ||
            (current->arg2 && operands_equal(current->arg2, inst->result)))
        {
            return false; // 被使用，不是死代码
        }

        // 如果有其他指令重新定义了相同的变量，则停止搜索
        if (current->result && operands_equal(current->result, inst->result))
        {
            break;
        }

        current = current->next;
    }

    return true; // 未被使用，是死代码
}

// 检查是否为冗余赋值
bool is_redundant_assignment(Instruction *inst)
{
    if (inst == NULL || inst->op != OP_ASSIGN)
        return false;

    // x = x 形式的赋值是冗余的
    return operands_equal(inst->result, inst->arg1);
}

// 查找变量的前一次赋值
Instruction *find_previous_assignment(Operand *var)
{
    Instruction *inst = code_head;
    Instruction *last_assignment = NULL;

    while (inst != NULL)
    {
        if (inst->result && operands_equal(inst->result, var))
        {
            last_assignment = inst;
        }
        inst = inst->next;
    }

    return last_assignment;
}

// 删除指令
void remove_instruction(Instruction *inst)
{
    if (inst == NULL)
        return;

    // 找到前一条指令
    Instruction *prev = NULL;
    Instruction *current = code_head;

    while (current != NULL && current != inst)
    {
        prev = current;
        current = current->next;
    }

    if (current == NULL)
        return; // 指令不在链表中

    // 更新链表连接
    if (prev == NULL)
    {
        // 删除的是头节点
        code_head = inst->next;
    }
    else
    {
        prev->next = inst->next;
    }

    if (inst == code_tail)
    {
        code_tail = prev;
    }

    // 释放指令内存
    free_instruction(inst);
}

// 替换操作数引用
void replace_operand_references(Operand *old_op, Operand *new_op)
{
    Instruction *inst = code_head;

    while (inst != NULL)
    {
        if (inst->arg1 && operands_equal(inst->arg1, old_op))
        {
            free_operand(inst->arg1);
            // 根据新操作数的类型创建对应的操作数
            if (new_op->type == OPERAND_CONSTANT)
            {
                inst->arg1 = new_operand_constant_int(new_op->u.int_value);
            }
            else if (new_op->type == OPERAND_TEMP)
            {
                inst->arg1 = new_operand_temp();
                inst->arg1->u.temp_no = new_op->u.temp_no;
            }
            else if (new_op->type == OPERAND_VARIABLE)
            {
                inst->arg1 = new_operand_variable(new_op->u.name);
            }
        }
        if (inst->arg2 && operands_equal(inst->arg2, old_op))
        {
            free_operand(inst->arg2);
            // 根据新操作数的类型创建对应的操作数
            if (new_op->type == OPERAND_CONSTANT)
            {
                inst->arg2 = new_operand_constant_int(new_op->u.int_value);
            }
            else if (new_op->type == OPERAND_TEMP)
            {
                inst->arg2 = new_operand_temp();
                inst->arg2->u.temp_no = new_op->u.temp_no;
            }
            else if (new_op->type == OPERAND_VARIABLE)
            {
                inst->arg2 = new_operand_variable(new_op->u.name);
            }
        }
        inst = inst->next;
    }
}

// 常量折叠优化
void constant_folding()
{
    Instruction *inst = code_head;

    while (inst != NULL)
    {
        Instruction *next = inst->next; // 保存下一条指令，因为当前指令可能被修改

        if (inst->arg1 && inst->arg2 &&
            is_constant_operand(inst->arg1) && is_constant_operand(inst->arg2))
        {
            int val1 = get_constant_value(inst->arg1);
            int val2 = get_constant_value(inst->arg2);
            int result_val = 0;
            bool can_fold = true;

            switch (inst->op)
            {
            case OP_ADD:
                result_val = val1 + val2;
                break;
            case OP_SUB:
                result_val = val1 - val2;
                break;
            case OP_MUL:
                result_val = val1 * val2;
                break;
            case OP_DIV:
                if (val2 != 0)
                    result_val = val1 / val2;
                else
                    can_fold = false;
                break;
            case OP_GT:
                result_val = val1 > val2 ? 1 : 0;
                break;
            case OP_LT:
                result_val = val1 < val2 ? 1 : 0;
                break;
            case OP_GE:
                result_val = val1 >= val2 ? 1 : 0;
                break;
            case OP_LE:
                result_val = val1 <= val2 ? 1 : 0;
                break;
            case OP_EQ:
                result_val = val1 == val2 ? 1 : 0;
                break;
            case OP_NE:
                result_val = val1 != val2 ? 1 : 0;
                break;
            default:
                can_fold = false;
                break;
            }

            if (can_fold)
            {
                // 将指令转换为赋值指令
                inst->op = OP_ASSIGN;
                free_operand(inst->arg1);
                free_operand(inst->arg2);
                inst->arg1 = new_operand_constant_int(result_val);
                inst->arg2 = NULL;
                opt_stats.constant_folding_count++;
            }
        }
        // 处理单操作数的常量折叠
        else if (inst->arg1 && is_constant_operand(inst->arg1) && inst->arg2 == NULL)
        {
            int val = get_constant_value(inst->arg1);
            int result_val = 0;
            bool can_fold = true;

            switch (inst->op)
            {
            case OP_NEG:
                result_val = -val;
                break;
            case OP_NOT:
                result_val = !val ? 1 : 0;
                break;
            default:
                can_fold = false;
                break;
            }

            if (can_fold)
            {
                inst->op = OP_ASSIGN;
                free_operand(inst->arg1);
                inst->arg1 = new_operand_constant_int(result_val);
                opt_stats.constant_folding_count++;
            }
        }

        inst = next;
    }
}

// 常量传播优化
void constant_propagation()
{
    Instruction *inst = code_head;

    while (inst != NULL)
    {
        // 查找形如 x = constant 的赋值
        if (inst->op == OP_ASSIGN && inst->arg1 && is_constant_operand(inst->arg1))
        {
            Operand *var = inst->result;
            Operand *constant = inst->arg1;

            // 在后续指令中替换对该变量的引用
            Instruction *next_inst = inst->next;
            int propagate_limit = 1000; // 防止无限循环，最多传播1000条指令
            int propagate_count = 0;

            while (next_inst != NULL && propagate_count < propagate_limit)
            {
                propagate_count++;

                // 如果变量被重新赋值，停止传播
                if (next_inst->result && operands_equal(next_inst->result, var))
                {
                    break;
                }

                // 替换对该变量的使用
                bool replaced = false;
                if (next_inst->arg1 && operands_equal(next_inst->arg1, var))
                {
                    free_operand(next_inst->arg1);
                    next_inst->arg1 = new_operand_constant_int(get_constant_value(constant));
                    replaced = true;
                }
                if (next_inst->arg2 && operands_equal(next_inst->arg2, var))
                {
                    free_operand(next_inst->arg2);
                    next_inst->arg2 = new_operand_constant_int(get_constant_value(constant));
                    replaced = true;
                }

                if (replaced)
                {
                    opt_stats.constant_propagation_count++;
                }

                next_inst = next_inst->next;
            }
        }

        inst = inst->next;
    }
}

// 死代码消除优化
void dead_code_elimination()
{
    bool changed = true;
    int max_iterations = 100; // 防止死循环，最多迭代100次
    int iteration_count = 0;

    while (changed && iteration_count < max_iterations)
    {
        changed = false;
        iteration_count++;
        Instruction *inst = code_head;

        while (inst != NULL)
        {
            Instruction *next = inst->next;

            if (is_dead_instruction(inst))
            {
                remove_instruction(inst);
                opt_stats.dead_code_elimination_count++;
                changed = true;
            }

            inst = next;
        }
    }

    if (iteration_count >= max_iterations)
    {
        printf("Warning: Dead code elimination reached maximum iterations (%d)\n", max_iterations);
    }
}

// 冗余赋值消除
void redundant_assignment_elimination()
{
    Instruction *inst = code_head;

    while (inst != NULL)
    {
        Instruction *next = inst->next;

        if (is_redundant_assignment(inst))
        {
            remove_instruction(inst);
            opt_stats.redundant_assignment_count++;
        }

        inst = next;
    }
}

// 公共子表达式消除
void common_subexpression_elimination()
{
    Instruction *inst1 = code_head;

    while (inst1 != NULL)
    {
        // 查找计算表达式的指令
        if (inst1->op >= OP_ADD && inst1->op <= OP_OR && inst1->arg1 && inst1->arg2)
        {
            Instruction *inst2 = inst1->next;
            int cse_limit = 1000; // 防止无限循环，最多搜索1000条指令
            int cse_count = 0;

            while (inst2 != NULL && cse_count < cse_limit)
            {
                cse_count++;

                // 查找相同的表达式
                if (inst2->op == inst1->op &&
                    operands_equal(inst2->arg1, inst1->arg1) &&
                    operands_equal(inst2->arg2, inst1->arg2))
                {
                    // 替换inst2的结果为inst1的结果
                    replace_operand_references(inst2->result, inst1->result);

                    // 将inst2转换为赋值指令
                    inst2->op = OP_ASSIGN;
                    free_operand(inst2->arg1);
                    free_operand(inst2->arg2);

                    // 正确创建操作数
                    if (inst1->result->type == OPERAND_TEMP)
                    {
                        inst2->arg1 = new_operand_temp();
                        inst2->arg1->u.temp_no = inst1->result->u.temp_no;
                    }
                    else if (inst1->result->type == OPERAND_VARIABLE)
                    {
                        inst2->arg1 = new_operand_variable(inst1->result->u.name);
                    }
                    else
                    {
                        inst2->arg1 = new_operand_constant_int(inst1->result->u.int_value);
                    }
                    inst2->arg2 = NULL;

                    opt_stats.common_subexpression_count++;
                }

                // 如果操作数被重新定义，停止搜索
                if ((inst2->result && operands_equal(inst2->result, inst1->arg1)) ||
                    (inst2->result && operands_equal(inst2->result, inst1->arg2)))
                {
                    break;
                }

                inst2 = inst2->next;
            }
        }

        inst1 = inst1->next;
    }
}

// 数组访问优化
void array_access_optimization()
{
    Instruction *inst = code_head;

    while (inst != NULL)
    {
        // 查找重复的数组访问模式
        if (inst->op == OP_ARRAY_GET)
        {
            Instruction *next_inst = inst->next;
            int array_limit = 1000; // 防止无限循环，最多搜索1000条指令
            int array_count = 0;

            while (next_inst != NULL && array_count < array_limit)
            {
                array_count++;

                // 查找相同的数组访问
                if (next_inst->op == OP_ARRAY_GET &&
                    operands_equal(next_inst->arg1, inst->arg1) &&
                    operands_equal(next_inst->arg2, inst->arg2))
                {
                    // 用第一次访问的结果替换第二次访问
                    replace_operand_references(next_inst->result, inst->result);

                    // 删除重复的数组访问
                    Instruction *to_remove = next_inst;
                    next_inst = next_inst->next;
                    remove_instruction(to_remove);
                    opt_stats.array_access_optimization_count++;
                    continue;
                }

                // 如果数组被修改，停止优化
                if (next_inst->op == OP_ARRAY_SET &&
                    operands_equal(next_inst->result, inst->arg1))
                {
                    break;
                }

                next_inst = next_inst->next;
            }
        }

        inst = inst->next;
    }
}

// 优化函数
void optimize_code()
{
    init_optimization_stats();
    opt_stats.total_instructions_before = count_instructions();

    printf("=== Starting Code Optimization ===\n");
    printf("Instructions before optimization: %d\n", opt_stats.total_instructions_before);

    // 第一步：常量折叠
    printf("Applying constant folding...\n");
    Instruction *inst = code_head;
    while (inst != NULL)
    {
        // 简单的常量折叠：将 x + 0 优化为 x
        if (inst->op == OP_ADD && inst->arg2 && inst->arg2->type == OPERAND_CONSTANT && inst->arg2->u.int_value == 0)
        {
            inst->op = OP_ASSIGN;
            free_operand(inst->arg2);
            inst->arg2 = NULL;
            opt_stats.constant_folding_count++;
        }
        // 简单的常量折叠：将 x * 1 优化为 x
        else if (inst->op == OP_MUL && inst->arg2 && inst->arg2->type == OPERAND_CONSTANT && inst->arg2->u.int_value == 1)
        {
            inst->op = OP_ASSIGN;
            free_operand(inst->arg2);
            inst->arg2 = NULL;
            opt_stats.constant_folding_count++;
        }
        // 常量折叠：计算常量表达式
        else if ((inst->op == OP_ADD || inst->op == OP_MUL || inst->op == OP_SUB) &&
                 inst->arg1 && inst->arg1->type == OPERAND_CONSTANT &&
                 inst->arg2 && inst->arg2->type == OPERAND_CONSTANT)
        {
            int val1 = inst->arg1->u.int_value;
            int val2 = inst->arg2->u.int_value;
            int result_val = 0;

            if (inst->op == OP_ADD)
                result_val = val1 + val2;
            else if (inst->op == OP_SUB)
                result_val = val1 - val2;
            else if (inst->op == OP_MUL)
                result_val = val1 * val2;

            inst->op = OP_ASSIGN;
            free_operand(inst->arg1);
            free_operand(inst->arg2);
            inst->arg1 = new_operand_constant_int(result_val);
            inst->arg2 = NULL;
            opt_stats.constant_folding_count++;
        }

        inst = inst->next;
    }

    // 第二步：基本的常量传播
    printf("Applying basic constant propagation...\n");
    inst = code_head;
    while (inst != NULL)
    {
        // 查找 x := constant 形式的赋值
        if (inst->op == OP_ASSIGN && inst->arg1 && inst->arg1->type == OPERAND_CONSTANT)
        {
            Operand *var = inst->result;
            int constant_value = inst->arg1->u.int_value;

            // 在紧接着的下一条指令中查找对该变量的使用
            Instruction *next_inst = inst->next;
            if (next_inst && next_inst->arg1 && operands_equal(next_inst->arg1, var))
            {
                // 替换变量为常量
                free_operand(next_inst->arg1);
                next_inst->arg1 = new_operand_constant_int(constant_value);
                opt_stats.constant_propagation_count++;
            }
            if (next_inst && next_inst->arg2 && operands_equal(next_inst->arg2, var))
            {
                // 替换变量为常量
                free_operand(next_inst->arg2);
                next_inst->arg2 = new_operand_constant_int(constant_value);
                opt_stats.constant_propagation_count++;
            }
        }

        inst = inst->next;
    }

    // 第三步：冗余赋值消除
    printf("Applying redundant assignment elimination...\n");
    inst = code_head;
    while (inst != NULL)
    {
        Instruction *next = inst->next;

        // 查找 x := y 后紧跟 z := x 的模式
        if (inst->op == OP_ASSIGN && next && next->op == OP_ASSIGN &&
            operands_equal(next->arg1, inst->result))
        {
            // 将 z := x 改为 z := y
            free_operand(next->arg1);
            if (inst->arg1->type == OPERAND_CONSTANT)
            {
                next->arg1 = new_operand_constant_int(inst->arg1->u.int_value);
            }
            else if (inst->arg1->type == OPERAND_VARIABLE)
            {
                next->arg1 = new_operand_variable(inst->arg1->u.name);
            }
            else if (inst->arg1->type == OPERAND_TEMP)
            {
                next->arg1 = new_operand_temp();
                next->arg1->u.temp_no = inst->arg1->u.temp_no;
            }
            opt_stats.redundant_assignment_count++;
        }

        inst = inst->next;
    }

    // 第四步：安全的死代码消除
    printf("Applying dead code elimination...\n");

    // 使用标记-清除方法，避免在遍历时修改链表
    bool found_dead_code = true;
    int dce_iterations = 0;
    int max_dce_iterations = 10; // 最多迭代10次

    while (found_dead_code && dce_iterations < max_dce_iterations)
    {
        found_dead_code = false;
        dce_iterations++;

        // 第一遍：标记死代码
        inst = code_head;
        while (inst != NULL)
        {
            if (inst->result && inst->op == OP_ASSIGN)
            {
                // 检查结果是否被使用
                bool is_used = false;
                Instruction *check_inst = inst->next;
                int check_count = 0;

                while (check_inst != NULL && check_count < 100) // 限制搜索范围
                {
                    check_count++;

                    if ((check_inst->arg1 && operands_equal(check_inst->arg1, inst->result)) ||
                        (check_inst->arg2 && operands_equal(check_inst->arg2, inst->result)))
                    {
                        is_used = true;
                        break;
                    }
                    if (check_inst->op == OP_RETURN && check_inst->result &&
                        operands_equal(check_inst->result, inst->result))
                    {
                        is_used = true;
                        break;
                    }
                    check_inst = check_inst->next;
                }

                if (!is_used)
                {
                    // 将死代码转换为注释（通过修改操作码为一个无害的值）
                    // 我们可以重用 OP_LABEL 作为"死代码"标记，因为它不会影响执行
                    if (inst->op != OP_LABEL) // 避免重复标记
                    {
                        inst->op = OP_LABEL; // 标记为死代码
                        found_dead_code = true;
                        opt_stats.dead_code_elimination_count++;
                    }
                }
            }
            inst = inst->next;
        }
    }

    if (dce_iterations >= max_dce_iterations)
    {
        printf("Dead code elimination stopped after %d iterations.\n", max_dce_iterations);
    }

    opt_stats.total_instructions_after = count_instructions();

    printf("Optimization completed.\n");
    print_optimization_stats();
}