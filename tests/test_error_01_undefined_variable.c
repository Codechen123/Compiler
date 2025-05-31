// 错误类型1：变量在使用时未经定义
int main()
{
    int a;
    a = b + 5; // 错误：变量b未定义
    return a;
}