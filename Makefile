CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -Wall -g -I$(SRCDIR)

# 源文件目录
SRCDIR = src

# 目标文件
TARGET = parser
OBJS = parser.tab.o lex.yy.o tree.o semantic.o codegen.o main.o

# 默认目标
all: $(TARGET)

# 生成最终的可执行文件
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

# 生成语法分析器
parser.tab.c parser.tab.h: $(SRCDIR)/parser.y
	$(BISON) -d $(SRCDIR)/parser.y

# 生成词法分析器
lex.yy.c: $(SRCDIR)/lexer.l parser.tab.h
	$(FLEX) $(SRCDIR)/lexer.l

# 编译各个源文件
parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

tree.o: $(SRCDIR)/tree.c $(SRCDIR)/tree.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/tree.c

semantic.o: $(SRCDIR)/semantic.c $(SRCDIR)/semantic.h $(SRCDIR)/tree.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/semantic.c

codegen.o: $(SRCDIR)/codegen.c $(SRCDIR)/codegen.h $(SRCDIR)/tree.h $(SRCDIR)/semantic.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/codegen.c

main.o: $(SRCDIR)/main.c $(SRCDIR)/tree.h $(SRCDIR)/semantic.h $(SRCDIR)/codegen.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/main.c

.PHONY: clean test
# 清理中间文件和目标文件
clean:
	rm -f $(TARGET) $(OBJS) lex.yy.c parser.tab.c parser.tab.h *~

# 运行测试
test: $(TARGET)
	@echo "运行基础功能测试..."
	@cd scripts && ./run_tests.bat
	@echo "运行优化功能测试..."
	@cd scripts && ./run_optimization_tests.bat
