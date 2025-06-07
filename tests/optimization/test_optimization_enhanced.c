int main()
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int result;

    a = 5;          // 常量
    b = 3;          // 常量
    c = a + b;      // 5 + 3 = 8 (常量折叠)
    d = c;          // 常量传播: d = 8
    e = d * 1;      // 8 * 1 = 8 (简化)
    result = e + 0; // 8 + 0 = 8 (简化)

    return result;
}