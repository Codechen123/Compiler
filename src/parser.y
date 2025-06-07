%{
#include <stdio.h>
#include "tree.h"

extern int yylineno;
extern char* yytext;
void yyerror(const char* msg);
int yylex();

TreeNode* root = NULL;  // 语法树根节点
int has_syntax_error = 0;  // 添加全局变量跟踪语法错误

void yyerror(const char* msg) {
    has_syntax_error = 1;  // 设置语法错误标志
    fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}
%}

%union {
    TreeNode* node;
}

/* 终结符 */
%token <node> INT FLOAT ID
%token <node> SEMI COMMA ASSIGNOP RELOP
%token <node> PLUS MINUS STAR DIV
%token <node> AND OR DOT NOT
%token <node> TYPE STRUCT RETURN IF ELSE WHILE
%token <node> LP RP LB RB LC RC

/* 非终结符 */
%type <node> Program ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier OptTag Tag
%type <node> VarDec FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt DefList Def DecList Dec
%type <node> Exp Args

/* 优先级和结合性 */
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT LB RB LP RP

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

/* Program */
Program: ExtDefList                { $$ = create_node(NODE_PROGRAM, @$.first_line, "Program"); 
                                   add_child($$, $1);
                                   root = $$; }
    ;

/* External Definitions */
ExtDefList: ExtDef ExtDefList     { $$ = create_node(NODE_EXTDEFLIST, @$.first_line, "ExtDefList");
                                   add_child($$, $1);
                                   if ($2) add_sibling($1, $2); }
    |                             { $$ = NULL; }
    ;

ExtDef: Specifier ExtDecList SEMI { $$ = create_node(NODE_EXTDEF, @$.first_line, "ExtDef");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | Specifier SEMI             { $$ = create_node(NODE_EXTDEF, @$.first_line, "ExtDef");
                                   add_child($$, $1);
                                   add_sibling($1, $2); }
    | Specifier FunDec CompSt    { $$ = create_node(NODE_EXTDEF, @$.first_line, "ExtDef");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    ;

ExtDecList: VarDec               { $$ = create_node(NODE_EXTDECLIST, @$.first_line, "ExtDecList");
                                   add_child($$, $1); }
    | VarDec COMMA ExtDecList    { $$ = create_node(NODE_EXTDECLIST, @$.first_line, "ExtDecList");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    ;

/* Specifiers */
Specifier: TYPE                  { $$ = create_node(NODE_SPECIFIER, @$.first_line, "Specifier");
                                   add_child($$, $1); }
    | StructSpecifier           { $$ = create_node(NODE_SPECIFIER, @$.first_line, "Specifier");
                                   add_child($$, $1); }
    ;

StructSpecifier: 
    STRUCT OptTag LC DefList RC { $$ = create_node(NODE_STRUCTSPECIFIER, @$.first_line, "StructSpecifier");
                                   add_child($$, $1);
                                   if ($2) add_sibling($1, $2);
                                   add_sibling($2 ? $2 : $1, $3);
                                   if ($4) add_sibling($3, $4);
                                   add_sibling($4 ? $4 : $3, $5); }
    | STRUCT Tag               { $$ = create_node(NODE_STRUCTSPECIFIER, @$.first_line, "StructSpecifier");
                                   add_child($$, $1);
                                   add_sibling($1, $2); }
    ;

OptTag: ID                      { $$ = create_node(NODE_OPTTAG, @$.first_line, "OptTag");
                                   add_child($$, $1); }
    |                          { $$ = NULL; }
    ;

Tag: ID                        { $$ = create_node(NODE_TAG, @$.first_line, "Tag");
                                   add_child($$, $1); }
    ;

/* Declarators */
VarDec: ID                     { $$ = create_node(NODE_VARDEC, @$.first_line, "VarDec");
                                   add_child($$, $1); }
    | VarDec LB INT RB        { $$ = create_node(NODE_VARDEC, @$.first_line, "VarDec");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3);
                                   add_sibling($3, $4); }
    ;

FunDec: ID LP VarList RP      { $$ = create_node(NODE_FUNDEC, @$.first_line, "FunDec");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3);
                                   add_sibling($3, $4); }
    | ID LP RP               { $$ = create_node(NODE_FUNDEC, @$.first_line, "FunDec");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    ;

VarList: ParamDec COMMA VarList { $$ = create_node(NODE_VARLIST, @$.first_line, "VarList");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | ParamDec                 { $$ = create_node(NODE_VARLIST, @$.first_line, "VarList");
                                   add_child($$, $1); }
    ;

ParamDec: Specifier VarDec    { $$ = create_node(NODE_PARAMDEC, @$.first_line, "ParamDec");
                                   add_child($$, $1);
                                   add_sibling($1, $2); }
    ;

/* Statements */
CompSt: LC DefList StmtList RC { $$ = create_node(NODE_COMPST, @$.first_line, "CompSt");
                                   add_child($$, $1);
                                   if ($2) add_sibling($1, $2);
                                   if ($3) add_sibling($2 ? $2 : $1, $3);
                                   add_sibling($3 ? $3 : ($2 ? $2 : $1), $4); }
    ;

StmtList: Stmt StmtList       { $$ = create_node(NODE_STMTLIST, @$.first_line, "StmtList");
                                   add_child($$, $1);
                                   if ($2) add_sibling($1, $2); }
    |                         { $$ = NULL; }
    ;

Stmt: Exp SEMI               { $$ = create_node(NODE_STMT, @$.first_line, "Stmt");
                                   add_child($$, $1);
                                   add_sibling($1, $2); }
    | CompSt                { $$ = create_node(NODE_STMT, @$.first_line, "Stmt");
                                   add_child($$, $1); }
    | RETURN Exp SEMI      { $$ = create_node(NODE_STMT, @$.first_line, "Stmt");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
                           { $$ = create_node(NODE_STMT, @$.first_line, "Stmt");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3);
                                   add_sibling($3, $4);
                                   add_sibling($4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt
                           { $$ = create_node(NODE_STMT, @$.first_line, "Stmt");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3);
                                   add_sibling($3, $4);
                                   add_sibling($4, $5);
                                   add_sibling($5, $6);
                                   add_sibling($6, $7); }
    | WHILE LP Exp RP Stmt { $$ = create_node(NODE_STMT, @$.first_line, "Stmt");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3);
                                   add_sibling($3, $4);
                                   add_sibling($4, $5); }
    ;

/* Local Definitions */
DefList: Def DefList        { $$ = create_node(NODE_DEFLIST, @$.first_line, "DefList");
                                   add_child($$, $1);
                                   if ($2) add_sibling($1, $2); }
    |                      { $$ = NULL; }
    ;

Def: Specifier DecList SEMI { $$ = create_node(NODE_DEF, @$.first_line, "Def");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    ;

DecList: Dec               { $$ = create_node(NODE_DECLIST, @$.first_line, "DecList");
                                   add_child($$, $1); }
    | Dec COMMA DecList   { $$ = create_node(NODE_DECLIST, @$.first_line, "DecList");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    ;

Dec: VarDec              { $$ = create_node(NODE_DEC, @$.first_line, "Dec");
                                   add_child($$, $1); }
    | VarDec ASSIGNOP Exp { $$ = create_node(NODE_DEC, @$.first_line, "Dec");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    ;

/* Expressions */
Exp: Exp ASSIGNOP Exp    { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | Exp AND Exp       { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | Exp OR Exp        { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | Exp RELOP Exp     { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | Exp PLUS Exp      { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | Exp MINUS Exp     { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | Exp STAR Exp      { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | Exp DIV Exp       { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | LP Exp RP         { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | MINUS Exp         { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2); }
    | NOT Exp           { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2); }
    | ID LP Args RP     { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3);
                                   add_sibling($3, $4); }
    | ID LP RP          { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | Exp LB Exp RB     { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3);
                                   add_sibling($3, $4); }
    | Exp DOT ID        { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | ID                { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1); }
    | INT               { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1); }
    | FLOAT             { $$ = create_node(NODE_EXP, @$.first_line, "Exp");
                                   add_child($$, $1); }
    ;

Args: Exp COMMA Args    { $$ = create_node(NODE_ARGS, @$.first_line, "Args");
                                   add_child($$, $1);
                                   add_sibling($1, $2);
                                   add_sibling($2, $3); }
    | Exp              { $$ = create_node(NODE_ARGS, @$.first_line, "Args");
                                   add_child($$, $1); }
    ;

%%