// 错误类型2：函数在调用时未经定义
int main()
{
    int x = 10;
    int result = foo(x); // 错误：函数foo未定义
    return result;
}