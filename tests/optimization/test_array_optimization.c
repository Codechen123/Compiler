int main()
{
    int arr[10];
    int i;
    int temp1;
    int temp2;
    int result;

    // 数组赋值
    i = 0;
    arr[0] = 5;
    arr[1] = 10;

    // 重复的数组访问
    temp1 = arr[0]; // 第一次访问 arr[0]
    temp2 = arr[0]; // 重复访问 arr[0]

    // 常量索引优化
    result = temp1 + temp2; // 应该可以优化为 result = arr[0] + arr[0]

    return result;
}