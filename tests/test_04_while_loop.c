// 测试用例4: while循环
int main()
{
    int i;
    int sum;
    i = 1;
    sum = 0;
    // 计算1到5的和
    while (i <= 5)
    {
        sum = sum + i;
        i = i + 1;
    }
    return sum;
}