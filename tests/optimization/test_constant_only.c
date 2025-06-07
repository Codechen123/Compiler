int main()
{
    int a;
    int b;
    int c;
    int result;

    // 应该被常量折叠和传播
    a = 5;
    b = 10;
    c = a + b;  // 5 + 10 = 15
    result = c; // 应该被优化为 result = 15

    return result;
}