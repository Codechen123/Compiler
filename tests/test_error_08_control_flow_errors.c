// 控制流语句中的错误测试
int check_condition(int x)
{
    if (x > undefined_var) // 错误1：undefined_var未定义
    {
        return 1.5; // 错误5：返回类型不匹配（float->int）
    }
    else
    {
        return x;
    }
}

int main()
{
    int i = 0;
    int i = 1; // 错误3：变量i重复定义

    while (i < unknown_limit) // 错误1：unknown_limit未定义
    {
        int result = undefined_function(i); // 错误2：undefined_function未定义
        i = i + 1;
    }

    return check_condition(i, 10); // 错误6：参数数目不匹配
}