@echo off
echo ========================================
echo 中间代码优化功能批量测试
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

echo 开始测试...
echo.

set test_files=test_optimization_enhanced.c test_advanced_optimization.c test_arithmetic_optimization.c test_chain_optimization.c test_array_optimization.c

for %%f in (%test_files%) do (
    echo ========================================
    echo 测试文件: %%f
    echo ========================================
    echo.
    
    echo --- 不启用优化 ---
    ./parser.exe %%f > temp_no_opt.txt 2>&1
    echo 生成文件: output.ir
    echo.
    
    echo --- 启用优化 ---
    ./parser.exe -O %%f
    echo 生成文件: output_optimized.ir
    echo.
    
    echo 按任意键继续下一个测试...
    pause >nul
    echo.
)

echo ========================================
echo 所有测试完成！
echo ========================================
echo.

echo 查看测试报告: 测试报告_中间代码优化.md
echo.

pause 