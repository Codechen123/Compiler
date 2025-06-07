int main()
{
    int matrix[3][3];
    int i;
    int j;
    int temp1;
    int temp2;
    int result;

    // 多维数组赋值
    i = 0;
    j = 1;
    matrix[0][1] = 5;
    matrix[1][0] = 10;

    // 常量优化机会
    temp1 = i + 0; // 应该优化为 temp1 = i
    temp2 = j * 1; // 应该优化为 temp2 = j

    // 多维数组访问
    result = matrix[0][1] + matrix[1][0];

    return result;
}