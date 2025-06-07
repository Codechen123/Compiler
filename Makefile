CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -Wall -g

# 目标文件
TARGET = parser
OBJS = parser.tab.o lex.yy.o tree.o semantic.o codegen.o main.o

# 默认目标
all: $(TARGET)

# 生成最终的可执行文件
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

# 生成语法分析器
parser.tab.c parser.tab.h: parser.y
	$(BISON) -d parser.y

# 生成词法分析器
lex.yy.c: lexer.l parser.tab.h
	$(FLEX) lexer.l

# 编译各个源文件
parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

tree.o: tree.c tree.h
	$(CC) $(CFLAGS) -c tree.c

semantic.o: semantic.c semantic.h tree.h
	$(CC) $(CFLAGS) -c semantic.c

codegen.o: codegen.c codegen.h tree.h semantic.h
	$(CC) $(CFLAGS) -c codegen.c

main.o: main.c tree.h semantic.h codegen.h
	$(CC) $(CFLAGS) -c main.c

.PHONY: clean
# 清理中间文件和目标文件
clean:
	rm -f $(TARGET) $(OBJS) *~
