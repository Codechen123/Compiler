int main()
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
    int result;

    // 测试长链的赋值优化
    a = 42;
    b = a;      // b = 42
    c = b;      // c = 42 (应该优化为 c = a)
    d = c;      // d = 42 (应该优化为 d = a)
    e = d;      // e = 42 (应该优化为 e = a)
    f = e;      // f = 42 (应该优化为 f = a)
    g = f;      // g = 42 (应该优化为 g = a)
    result = g; // result = 42 (应该优化为 result = a)

    return result;
}