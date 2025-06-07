@echo off
echo ========================================
echo 中间代码指令减少功能演示
echo ========================================
echo.

echo 编译编译器...
make
if %errorlevel% neq 0 (
    echo 编译失败！
    pause
    exit /b 1
)
echo.

echo ========================================
echo 测试1: 简单减少测试 (最佳效果 57.1%%)
echo ========================================
echo.
echo 源代码:
echo int main() {
echo     int a, b, unused, result;
echo     a = 10; result = a;
echo     unused = 100; b = 200;  // 死代码
echo     return result;
echo }
echo.
echo --- 优化结果 ---
./parser.exe -O test_simple_reduction.c
echo.
pause

echo ========================================
echo 测试2: 死代码消除测试 (减少率 46.2%%)
echo ========================================
echo.
echo --- 优化结果 ---
./parser.exe -O test_dead_code_elimination.c
echo.
pause

echo ========================================
echo 测试3: 常量优化测试 (减少率 37.5%%)
echo ========================================
echo.
echo --- 优化结果 ---
./parser.exe -O test_constant_only.c
echo.

echo ========================================
echo 演示完成！查看详细报告:
echo - 指令减少测试报告.md
echo ========================================
pause 