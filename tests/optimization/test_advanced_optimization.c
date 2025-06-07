int main()
{
    int x;
    int y;
    int z;
    int result1;
    int result2;
    int result3;

    // 测试复杂的常量折叠
    x = 10;
    y = 5;
    z = x + y;       // 10 + 5 = 15
    result1 = z - 3; // 15 - 3 = 12 (如果常量传播生效)

    // 测试多层常量传播
    result2 = result1 * 1; // 应该简化为 result2 = result1
    result3 = result2 + 0; // 应该简化为 result3 = result2

    return result3;
}