@echo off

set COMPILER=.\parser.exe
set TEST_DIR=tests
set RESULT_DIR=test_results

if not exist %RESULT_DIR% mkdir %RESULT_DIR%

echo === 编译器测试开始 ===

echo 构建编译器...
make

if %errorlevel% neq 0 (
    echo 编译器构建失败!
    pause
    exit /b 1
)

if not exist %RESULT_DIR% mkdir %RESULT_DIR%

echo.
echo === 运行测试用例 ===

echo 测试1: 基本变量声明和赋值
%COMPILER% %TEST_DIR%\test_01_basic_variables.c > %RESULT_DIR%\test_01_output.txt 2>&1

echo 测试2: 算术运算
%COMPILER% %TEST_DIR%\test_02_arithmetic.c > %RESULT_DIR%\test_02_output.txt 2>&1

echo 测试3: if语句
%COMPILER% %TEST_DIR%\test_03_if_statement.c > %RESULT_DIR%\test_03_output.txt 2>&1

echo 测试4: while循环
%COMPILER% %TEST_DIR%\test_04_while_loop.c > %RESULT_DIR%\test_04_output.txt 2>&1

echo 测试5: 函数定义和调用
%COMPILER% %TEST_DIR%\test_05_functions.c > %RESULT_DIR%\test_05_output.txt 2>&1

echo 测试6: 数组操作
%COMPILER% %TEST_DIR%\test_06_arrays.c > %RESULT_DIR%\test_06_output.txt 2>&1

echo 测试7: 结构体
%COMPILER% %TEST_DIR%\test_07_struct.c > %RESULT_DIR%\test_07_output.txt 2>&1

echo 测试8: 复杂表达式
%COMPILER% %TEST_DIR%\test_08_complex_expressions.c > %RESULT_DIR%\test_08_output.txt 2>&1

echo 测试9: 逻辑运算
%COMPILER% %TEST_DIR%\test_09_logical_operations.c > %RESULT_DIR%\test_09_output.txt 2>&1

echo 测试10: 嵌套控制结构
%COMPILER% %TEST_DIR%\test_10_nested_control.c > %RESULT_DIR%\test_10_output.txt 2>&1

echo.
echo === 错误测试用例 ===

echo.
echo 测试错误类型1：变量在使用时未经定义
%COMPILER% %TEST_DIR%\test_error_01_undefined_variable.c > %RESULT_DIR%\error_01_result.txt 2>&1
echo 结果保存到: %RESULT_DIR%\error_01_result.txt

echo.
echo 测试错误类型2：函数在调用时未经定义
%COMPILER% %TEST_DIR%\test_error_02_undefined_function.c > %RESULT_DIR%\error_02_result.txt 2>&1
echo 结果保存到: %RESULT_DIR%\error_02_result.txt

echo.
echo 测试错误类型3：变量出现重复定义
%COMPILER% %TEST_DIR%\test_error_03_duplicate_variable.c > %RESULT_DIR%\error_03_result.txt 2>&1
echo 结果保存到: %RESULT_DIR%\error_03_result.txt

echo.
echo 测试错误类型4：函数出现重复定义
%COMPILER% %TEST_DIR%\test_error_04_duplicate_function.c > %RESULT_DIR%\error_04_result.txt 2>&1
echo 结果保存到: %RESULT_DIR%\error_04_result.txt

echo.
echo 测试错误类型5：return语句的返回类型与函数定义的返回类型不匹配
%COMPILER% %TEST_DIR%\test_error_05_return_type_mismatch.c > %RESULT_DIR%\error_05_result.txt 2>&1
echo 结果保存到: %RESULT_DIR%\error_05_result.txt

echo.
echo 测试错误类型6：函数调用时实参与形参的数目或类型不匹配
%COMPILER% %TEST_DIR%\test_error_06_function_call_mismatch.c > %RESULT_DIR%\error_06_result.txt 2>&1
echo 结果保存到: %RESULT_DIR%\error_06_result.txt

echo.
echo 测试复合错误：包含多种错误类型
%COMPILER% %TEST_DIR%\test_error_07_multiple_errors.c > %RESULT_DIR%\error_07_result.txt 2>&1
echo 结果保存到: %RESULT_DIR%\error_07_result.txt

echo.
echo 测试控制流语句中的错误
%COMPILER% %TEST_DIR%\test_error_08_control_flow_errors.c > %RESULT_DIR%\error_08_result.txt 2>&1
echo 结果保存到: %RESULT_DIR%\error_08_result.txt

echo.
echo === 测试完成 ===
echo 详细结果保存在 %RESULT_DIR% 目录中
