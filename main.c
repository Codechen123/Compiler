#include <stdio.h>
#include "tree.h"
#include "semantic.h"
#include "codegen.h"

extern int yyparse();
extern void yyrestart(FILE *);
extern TreeNode *root;
extern int has_lexical_error; // 声明外部变量
extern int has_syntax_error;  // 声明语法错误变量

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        fprintf(stderr, "Usage: %s input_file\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }

    yyrestart(f);
    yyparse();
    fclose(f);

    // 只在没有词法错误和语法错误时进行语义分析和打印语法树
    if (!has_lexical_error && !has_syntax_error && root != NULL)
    {
        // 进行语义分析
        semantic_analysis(root);

        // 如果没有语义错误，打印语法树并生成中间代码
        if (semantic_error_count == 0)
        {
            printf("=== Syntax Tree ===\n");
            print_tree(root, 0);

            printf("\n=== Intermediate Code (Three-Address Code) ===\n");
            generate_code(root);
            print_code();

            // 可选：将中间代码保存到文件
            save_code_to_file("output.ir");
            printf("\nIntermediate code saved to output.ir\n");
        }
    }

    return 0;
}