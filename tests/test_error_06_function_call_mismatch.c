// 错误类型6：函数调用时实参与形参的数目或类型不匹配
int add(int x, int y)
{
    return x + y;
}

float multiply(float a, float b)
{
    return a * b;
}

int main()
{
    // 错误：参数数目不匹配 - add需要2个参数，但只提供了1个
    int result1 = add(5);

    // 错误：参数数目不匹配 - add需要2个参数，但提供了3个
    int result2 = add(1, 2, 3);

    int result3 = add(1, 2);

    // 错误：参数类型不匹配 - multiply需要float类型，但提供了int类型
    float result4 = multiply(10, 20);

    return 0;
}