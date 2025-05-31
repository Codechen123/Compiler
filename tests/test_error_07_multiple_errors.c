// 复合错误测试：包含多种错误类型
int func1(int x)
{
    return x + y; // 错误1：变量y未定义
}

int func1(float z) // 错误4：函数func1重复定义
{
    return z; // 错误5：返回类型不匹配（float->int）
}

int main()
{
    int a = 10;
    int a = 20; // 错误3：变量a重复定义

    int result1 = func2(a); // 错误2：函数func2未定义
    int result2 = func1();  // 错误6：参数数目不匹配

    return result1 + result2;
}