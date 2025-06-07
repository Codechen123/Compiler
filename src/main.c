#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "semantic.h"
#include "codegen.h"

extern int yyparse();
extern void yyrestart(FILE *);
extern TreeNode *root;
extern int has_lexical_error; // 声明外部变量
extern int has_syntax_error;  // 声明语法错误变量

void print_usage(const char *program_name)
{
    printf("Usage: %s [options] input_file\n", program_name);
    printf("Options:\n");
    printf("  -O, --optimize    Enable code optimization\n");
    printf("  -h, --help        Show this help message\n");
    printf("  -v, --verbose     Verbose output\n");
}

int main(int argc, char **argv)
{
    bool enable_optimization = false;
    bool verbose = false;
    char *input_file = NULL;

    // 解析命令行参数
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-O") == 0 || strcmp(argv[i], "--optimize") == 0)
        {
            enable_optimization = true;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
        {
            verbose = true;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            print_usage("parser");
            return 0;
        }
        else if (argv[i][0] != '-')
        {
            input_file = argv[i];
        }
        else
        {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage("parser");
            return 1;
        }
    }

    if (input_file == NULL)
    {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        return 1;
    }

    FILE *f = fopen(input_file, "r");
    if (!f)
    {
        perror(input_file);
        return 1;
    }

    if (verbose)
    {
        printf("Compiling: %s\n", input_file);
        printf("Optimization: %s\n", enable_optimization ? "enabled" : "disabled");
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
            if (verbose)
            {
                printf("=== Syntax Tree ===\n");
                print_tree(root, 0);
            }

            printf("\n=== Intermediate Code (Three-Address Code) ===\n");
            generate_code(root);

            if (enable_optimization)
            {
                printf("\n=== Code Before Optimization ===\n");
                print_code();

                // 执行优化
                optimize_code();

                printf("\n=== Code After Optimization ===\n");
                print_code();

                // 保存优化后的代码
                save_code_to_file("output_optimized.ir");
                printf("\nOptimized intermediate code saved to output_optimized.ir\n");
            }
            else
            {
                print_code();

                // 保存原始代码
                save_code_to_file("output.ir");
                printf("\nIntermediate code saved to output.ir\n");
            }
        }
    }

    return 0;
}