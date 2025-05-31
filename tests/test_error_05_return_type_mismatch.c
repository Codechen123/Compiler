// 错误类型5：return语句的返回类型与函数定义的返回类型不匹配
int getNumber()
{
    float x = 3.14;
    return x; // 错误：返回float类型，但函数定义为int类型
}

float getValue()
{
    int y = 10;
    return y; // 错误：返回int类型，但函数定义为float类型
}

int main()
{
    int a = getNumber();
    float b = getValue();
    return 0;
}