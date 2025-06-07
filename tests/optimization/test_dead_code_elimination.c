int main()
{
    int a;
    int b;
    int c;
    int d;
    int unused1;
    int unused2;
    int result;

    // 有用的代码
    a = 10;
    b = 20;
    result = a + b;

    // 死代码 - 这些变量从未被使用
    unused1 = 100;
    unused2 = unused1 * 2;
    c = 50;     // 计算了但从未使用
    d = c + 30; // 计算了但从未使用

    return result;
}