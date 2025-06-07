#include "tree.h"

TreeNode *create_node(NodeType type, int lineno, const char *name)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    if (node == NULL)
    {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }

    node->type = type;
    node->lineno = lineno;
    node->name = strdup(name);
    node->child = NULL;
    node->sibling = NULL;

    return node;
}

void add_child(TreeNode *parent, TreeNode *child)
{
    if (parent == NULL || child == NULL)
        return;
    parent->child = child;
}

void add_sibling(TreeNode *node, TreeNode *sibling)
{
    if (node == NULL)
        return;
    node->sibling = sibling;
}

void print_tree(TreeNode *root, int level)
{
    if (root == NULL)
        return;

    // 打印当前节点的缩进
    for (int i = 0; i < level; i++)
    {
        printf("  ");
    }

    // 根据节点类型打印不同的信息
    switch (root->type)
    {
    case NODE_TYPE:
        printf("TYPE: %s", root->name);
        break;
    case NODE_ID:
        printf("ID: %s", root->name);
        break;
    case NODE_INT:
        printf("INT: %d", root->value.int_value);
        break;
    case NODE_FLOAT:
        printf("FLOAT: %f", root->value.float_value);
        break;
    case NODE_SEMI:
    case NODE_COMMA:
    case NODE_ASSIGNOP:
    case NODE_RELOP:
    case NODE_PLUS:
    case NODE_MINUS:
    case NODE_STAR:
    case NODE_DIV:
    case NODE_AND:
    case NODE_OR:
    case NODE_DOT:
    case NODE_NOT:
    case NODE_LP:
    case NODE_RP:
    case NODE_LB:
    case NODE_RB:
    case NODE_LC:
    case NODE_RC:
        printf("%s", root->name);
        break;
    default:
        printf("%s", root->name);
        break;
    }

    // 打印行号（除了特定的token）
    switch (root->type)
    {
    case NODE_SEMI:
    case NODE_COMMA:
    case NODE_ASSIGNOP:
    case NODE_RELOP:
    case NODE_PLUS:
    case NODE_MINUS:
    case NODE_STAR:
    case NODE_DIV:
    case NODE_AND:
    case NODE_OR:
    case NODE_DOT:
    case NODE_NOT:
    case NODE_LP:
    case NODE_RP:
    case NODE_LB:
    case NODE_RB:
    case NODE_LC:
    case NODE_RC:
    case NODE_TYPE:
    case NODE_ID:
    case NODE_INT:
    case NODE_FLOAT:
    case NODE_STRUCT:
    case NODE_RETURN:
    case NODE_IF:
    case NODE_ELSE:
    case NODE_WHILE:
        break;
    default:
        printf(" (%d)", root->lineno);
        break;
    }
    printf("\n");

    // 递归打印子节点
    TreeNode *child = root->child;
    while (child != NULL)
    {
        print_tree(child, level + 1);
        child = child->sibling;
    }
}