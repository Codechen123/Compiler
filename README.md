# C 语言编译器实现

一个用 C 语言实现的编译器，支持词法分析、语法分析、语义分析和中间代码生成，能够将类 C 语言程序编译为三地址代码。

## 📋 项目概述

本项目是一个完整的编译器前端实现，包含编译器的核心阶段：

- **词法分析 (Lexical Analysis)** - 使用 Flex 生成词法分析器
- **语法分析 (Syntax Analysis)** - 使用 Bison 生成语法分析器
- **语义分析 (Semantic Analysis)** - 类型检查和符号表管理
- **中间代码生成 (Intermediate Code Generation)** - 生成三地址代码

## ✨ 支持的语言特性

### 数据类型

- 整型 (`int`)
- 浮点型 (`float`)
- 数组类型
- 结构体类型

### 语言构造

- **变量声明和定义**
- **函数定义和调用**
- **算术运算**: `+`, `-`, `*`, `/`
- **关系运算**: `>`, `<`, `>=`, `<=`, `==`, `!=`
- **逻辑运算**: `&&`, `||`, `!`
- **赋值运算**: `=`
- **控制流语句**:
  - `if-else` 条件语句
  - `while` 循环语句
  - `return` 返回语句

### 示例代码

```c
int add(int x, int y)
{
    return x + y;
}

int main()
{
    int a = 5;
    int b = 10;
    int c;
    c = a + b;
    return c;
}
```

## 🏗️ 项目结构

```
├── main.c           # 主程序入口
├── lexer.l          # Flex词法分析器规则
├── parser.y         # Bison语法分析器规则
├── tree.c/tree.h    # 抽象语法树实现
├── semantic.c/semantic.h  # 语义分析器
├── codegen.c/codegen.h    # 中间代码生成器
├── 文法定义.txt      # 语言文法定义
├── Makefile         # 构建配置
├── Test/           # 测试用例
│   └── test_params.c
└── README.md       # 项目说明
```

## 🔧 构建和安装

### 系统要求

- GCC 编译器
- Flex (词法分析器生成器)
- Bison (语法分析器生成器)
- Make 构建工具

### 构建步骤

1. **克隆或下载项目**

```bash
# 如果使用git
git clone <repository-url>
cd <project-directory>
```

2. **编译项目**

```bash
make
```

3. **清理构建文件**

```bash
make clean
```

## 🚀 使用方法

### 基本用法

```bash
./compiler <source_file>
```

### 示例

```bash
# 编译测试文件
./compiler Test/test_params.c
```

### 输出

编译器会输出三地址代码到控制台，例如：

```
FUNCTION main :
a := #5
b := #10
t3 := a + b
c := t3
RETURN c
END FUNCTION main
```

## 📊 三地址代码格式

生成的中间代码采用三地址代码格式，包含以下指令类型：

### 算术运算

- `x := y + z` (加法)
- `x := y - z` (减法)
- `x := y * z` (乘法)
- `x := y / z` (除法)

### 赋值操作

- `x := y` (赋值)
- `x := #5` (常量赋值)

### 控制流

- `GOTO label` (无条件跳转)
- `IF x GOTO label` (条件跳转)
- `label :` (标签定义)

### 函数操作

- `FUNCTION name :` (函数开始)
- `END FUNCTION name` (函数结束)
- `PARAM x` (参数声明)
- `ARG x` (参数传递)
- `x := CALL func` (函数调用)
- `RETURN x` (返回语句)

## 🧪 测试

项目包含测试用例验证编译器功能：

```bash
# 运行基本测试
./compiler Test/test_params.c
```

测试文件包含：

- 函数定义和调用
- 变量声明和赋值
- 算术表达式计算
- 返回语句

## 🔄 开发状态

### ✅ 已完成功能

- [x] 词法分析 - 完整的 token 识别
- [x] 语法分析 - 构建抽象语法树
- [x] 语义分析 - 类型检查和符号表
- [x] 基本表达式代码生成
- [x] 函数定义和调用
- [x] 算术运算
- [x] 变量赋值

### 🚧 待完善功能

- [ ] 条件语句代码生成优化
- [ ] 数组操作支持
- [ ] 结构体操作支持
- [ ] 错误恢复机制
- [ ] 代码优化

## 📝 语法规范

详细的语法定义请参考 `文法定义.txt` 文件，包含完整的 BNF 语法规则。

---

_最后更新: 2024 年_
