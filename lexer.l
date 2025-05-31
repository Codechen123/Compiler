%{
#include "tree.h"
#include "parser.tab.h"
#include <stdio.h>

int yycolumn = 1;
int has_lexical_error = 0;

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

%}

%option yylineno

delim       [ \t]
ws          {delim}+
newline     \n
digit       [0-9]
letter      [a-zA-Z]
id          ({letter}|_)({letter}|{digit}|_)*
int         {digit}+
float       {digit}+\.{digit}+

%%

{ws}        { /* ignore whitespace */ }
{newline}   { yycolumn = 1; }

"//".*      { /* ignore single-line comments */ }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* ignore multi-line comments */ }

"int"       { 
    yylval.node = create_node(NODE_TYPE, yylineno, "int"); 
    yylval.node->value.string_value = strdup("int");
    return TYPE; 
}
"float"     { 
    yylval.node = create_node(NODE_TYPE, yylineno, "float"); 
    yylval.node->value.string_value = strdup("float");
    return TYPE; 
}
"struct"    { yylval.node = create_node(NODE_STRUCT, yylineno, "STRUCT"); return STRUCT; }
"return"    { yylval.node = create_node(NODE_RETURN, yylineno, "RETURN"); return RETURN; }
"if"        { yylval.node = create_node(NODE_IF, yylineno, "IF"); return IF; }
"else"      { yylval.node = create_node(NODE_ELSE, yylineno, "ELSE"); return ELSE; }
"while"     { yylval.node = create_node(NODE_WHILE, yylineno, "WHILE"); return WHILE; }

";"         { yylval.node = create_node(NODE_SEMI, yylineno, "SEMI"); return SEMI; }
","         { yylval.node = create_node(NODE_COMMA, yylineno, "COMMA"); return COMMA; }
"="         { yylval.node = create_node(NODE_ASSIGNOP, yylineno, "ASSIGNOP"); return ASSIGNOP; }
">"         { yylval.node = create_node(NODE_RELOP, yylineno, ">"); return RELOP; }
"<"         { yylval.node = create_node(NODE_RELOP, yylineno, "<"); return RELOP; }
">="        { yylval.node = create_node(NODE_RELOP, yylineno, ">="); return RELOP; }
"<="        { yylval.node = create_node(NODE_RELOP, yylineno, "<="); return RELOP; }
"=="        { yylval.node = create_node(NODE_RELOP, yylineno, "=="); return RELOP; }
"!="        { yylval.node = create_node(NODE_RELOP, yylineno, "!="); return RELOP; }
"+"         { yylval.node = create_node(NODE_PLUS, yylineno, "PLUS"); return PLUS; }
"-"         { yylval.node = create_node(NODE_MINUS, yylineno, "MINUS"); return MINUS; }
"*"         { yylval.node = create_node(NODE_STAR, yylineno, "STAR"); return STAR; }
"/"         { yylval.node = create_node(NODE_DIV, yylineno, "DIV"); return DIV; }
"&&"        { yylval.node = create_node(NODE_AND, yylineno, "AND"); return AND; }
"||"        { yylval.node = create_node(NODE_OR, yylineno, "OR"); return OR; }
"."         { yylval.node = create_node(NODE_DOT, yylineno, "DOT"); return DOT; }
"!"         { yylval.node = create_node(NODE_NOT, yylineno, "NOT"); return NOT; }
"("         { yylval.node = create_node(NODE_LP, yylineno, "LP"); return LP; }
")"         { yylval.node = create_node(NODE_RP, yylineno, "RP"); return RP; }
"["         { yylval.node = create_node(NODE_LB, yylineno, "LB"); return LB; }
"]"         { yylval.node = create_node(NODE_RB, yylineno, "RB"); return RB; }
"{"         { yylval.node = create_node(NODE_LC, yylineno, "LC"); return LC; }
"}"         { yylval.node = create_node(NODE_RC, yylineno, "RC"); return RC; }

{id}        { 
    yylval.node = create_node(NODE_ID, yylineno, yytext);
    yylval.node->value.string_value = strdup(yytext);
    return ID; 
}

{int}       {
    yylval.node = create_node(NODE_INT, yylineno, "INT");
    yylval.node->value.int_value = atoi(yytext);
    return INT;
}

{float}     {
    yylval.node = create_node(NODE_FLOAT, yylineno, "FLOAT");
    yylval.node->value.float_value = atof(yytext);
    return FLOAT;
}

.           { 
    printf("Error type A at Line %d: Mysterious character \"%s\".\n", yylineno, yytext);
    has_lexical_error = 1;
}

%%

int yywrap() {
    return 1;
}