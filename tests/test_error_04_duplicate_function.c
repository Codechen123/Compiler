// 错误类型4：函数出现重复定义
int add(int x, int y)
{
    return x + y;
}

int add(int a, int b) // 错误：函数add重复定义
{
    return a + b;
}

int main()
{
    int result = add(3, 4);
    return result;
}