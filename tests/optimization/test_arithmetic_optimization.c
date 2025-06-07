int main()
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int result;

    // 测试各种算术优化
    a = 8;
    b = 4;

    // 常量折叠测试
    c = a + b; // 8 + 4 = 12
    d = a - b; // 8 - 4 = 4
    e = a * b; // 8 * 4 = 32

    // 恒等元优化测试
    f = c + 0;      // 应该优化为 f = c
    result = f * 1; // 应该优化为 result = f

    return result;
}