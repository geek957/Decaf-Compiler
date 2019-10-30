%{
#include <bits/stdc++.h>
#include "parser.tab.hpp"
#define YY_DECL extern "C" int yylex()
/*extern union Node yylval;*/
using namespace std;
int lineno = 0;
%}

number [0-9][0-9]*
id [a-zA-Z][0-9a-zA-Z]*
delim [ \t]+
char [ -~]
character   \'{char}\'
string (\\n|\\t|\\'|\\\\|\\\"|[^\\"'])
hex_number 0x[0-9a-fA-F][0-9a-fA-F]*


%%
"class"             {return CLASS;}
"Program"			{return PROGRAM;}
"int"				{yylval.str = strdup(yytext);return TYPE;}
"boolean"			{yylval.str = strdup(yytext);return TYPE;}
"void"				{yylval.str = strdup(yytext);return VOID;}
"if"				{return IF;}
"for"				{return FOR;}
"return"			{return RETURN;}
"break"				{return BREAK;}
"continue"			{return CONTINUE;}
"else"				{return ELSE;}
"callout"			{return CALLOUT;}
"true"				{yylval.str = strdup(yytext);return BOOL_LIT;}
"false"				{yylval.str = strdup(yytext);return BOOL_LIT;}
">="				{yylval.str = strdup(yytext);return GE;}
"<="				{yylval.str = strdup(yytext);return LE;}
"+="				{yylval.str = strdup(yytext);return PLE;}
"-="				{yylval.str = strdup(yytext);return SUE;}
"=="				{yylval.str = strdup(yytext);return EE;}
"!="				{yylval.str = strdup(yytext);return NE;}
"!"					{yylval.str = strdup(yytext);return EXCL;}
"="					{yylval.str = strdup(yytext);return '=';}
">"					{yylval.str = strdup(yytext);return GT;}
"<"					{yylval.str = strdup(yytext);return LT;}
"+"					{yylval.str = strdup(yytext);return PLUS;}
"-"					{yylval.str = strdup(yytext);return MINUS;}
"*"					{yylval.str = strdup(yytext);return MUL;}
"/"					{yylval.str = strdup(yytext);return DIV;}
"%"					{yylval.str = strdup(yytext);return MOD;}
"&&"				{yylval.str = strdup(yytext);return AND;}
"||"				{yylval.str = strdup(yytext);return OR;}
","					{return ',';}
"{"					{return '{';}
"}"					{return '}';}
"["					{return '[';}
"]"					{return ']';}
"("					{return '(';}
")"					{return ')';}
";"					{return ';';}
{number}			{yylval.number = atoi(yytext);return INT_LIT;}
{hex_number}		{yylval.number = atoi(yytext);return INT_LIT;}
{id}				{yylval.str = strdup(yytext);return ID;}
{character} 		{yylval.str = strdup(yytext);return CHAR;}
\"{string}*\" 		{yylval.str = strdup(yytext);return STRING;}
{delim}				/* Ignore whitespaces tabs and newlines */
"//".*				/* Inline comments So Ignore */
"\n"		     	{++lineno;}
.					{  }
%%

