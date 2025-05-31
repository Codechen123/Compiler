@echo off
echo === 编译器测试开始 ===

echo 构建编译器...
make

if %errorlevel% neq 0 (
    echo 编译器构建失败!
    pause
    exit /b 1
)

if not exist test_results mkdir test_results

echo.
echo === 运行测试用例 ===

echo 测试1: 基本变量声明和赋值
parser.exe tests\test_01_basic_variables.c > test_results\test_01_output.txt 2>&1

echo 测试2: 算术运算
parser.exe tests\test_02_arithmetic.c > test_results\test_02_output.txt 2>&1

echo 测试3: if语句
parser.exe tests\test_03_if_statement.c > test_results\test_03_output.txt 2>&1

echo 测试4: while循环
parser.exe tests\test_04_while_loop.c > test_results\test_04_output.txt 2>&1

echo 测试5: 函数定义和调用
parser.exe tests\test_05_functions.c > test_results\test_05_output.txt 2>&1

echo 测试6: 数组操作
parser.exe tests\test_06_arrays.c > test_results\test_06_output.txt 2>&1

echo 测试7: 结构体
parser.exe tests\test_07_struct.c > test_results\test_07_output.txt 2>&1

echo 测试8: 复杂表达式
parser.exe tests\test_08_complex_expressions.c > test_results\test_08_output.txt 2>&1

echo 测试9: 逻辑运算
parser.exe tests\test_09_logical_operations.c > test_results\test_09_output.txt 2>&1

echo 测试10: 嵌套控制结构
parser.exe tests\test_10_nested_control.c > test_results\test_10_output.txt 2>&1

echo.
echo === 错误测试用例 ===

echo 错误测试1: 未声明变量
parser.exe tests\test_error_01_undeclared_var.c > test_results\error_01_output.txt 2>&1

echo 错误测试2: 类型不匹配
parser.exe tests\test_error_02_type_mismatch.c > test_results\error_02_output.txt 2>&1

echo 错误测试3: 语法错误
parser.exe tests\test_error_03_syntax_error.c > test_results\error_03_output.txt 2>&1

echo.
echo === 测试完成 ===
echo 详细结果保存在 test_results 目录中
pause 