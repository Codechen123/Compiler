// 测试用例9: 逻辑运算
int main()
{
    int a;
    int b;
    int c;
    a = 5;
    b = 3;
    // 逻辑与运算
    if (a > b && b > 0)
    {
        c = 1;
    }
    else
    {
        c = 0;
    }
    // 逻辑或运算
    if (a < 0 || b < 0)
    {
        c = -1;
    }
    return c;
}