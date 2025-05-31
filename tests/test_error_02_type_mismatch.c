// 错误测试用例2: 类型不匹配
int main()
{
    int a;
    float b;
    a = 5;
    b = 3.14;
    a = b; // 可能的类型不匹配警告
    return a;
}