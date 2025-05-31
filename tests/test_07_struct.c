// 测试用例7: 结构体
/* 定义点结构体 */
struct Point
{
    int x;
    int y;
};

int main()
{
    struct Point p; // 声明结构体变量
    p.x = 10;       // 访问结构体成员
    p.y = 20;
    return p.x;
}