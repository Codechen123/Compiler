// 测试用例10: 嵌套控制结构
int main()
{
    int i;
    int j;
    int sum;
    sum = 0;
    i = 1;
    /* 嵌套循环 */
    while (i <= 3)
    {
        j = 1;
        while (j <= 2)
        {
            if (i > j)
            {
                sum = sum + i;
            }
            else
            {
                sum = sum + j;
            }
            j = j + 1;
        }
        i = i + 1;
    }
    return sum;
}