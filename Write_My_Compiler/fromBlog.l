%{
	#include <stdio.h>
	#include <stdlib.h>
	int count = 0;

%}
digit		    [0-9]
letter		    [a-zA-Z]
reservedWord	[w][h][i][l][e]|[i][f]|[e][l][s][e]|[s][w][i][t][c][h]|[c][a][s][e]|[i][n][t]|[m][a][i][n]|[u][s][i][n][g]|[n][a][m][e][s][p][a][c][e]|[s][t][d]|[p][r][i][n][t][f]
id	            ({letter}|_)({letter}|{digit}|_)*
num	            {digit}+
operator	    \+|-|\*|<=|<|==|=|>=|>|>>|<<
delim		    [ \t\n\r]
whitespace	    {delim}+
semicolon       [\;]
str             \"[^"]*\"
other		    .


%%
{reservedWord}  {count++;printf("%d\t(reserved word,\'%s\')\n",count,yytext);}
{id}            {count++;printf("%d\t(id,\'%s\')\n",count,yytext);}
{num}	        {count++;printf("%d\t(num,\'%s\')\n",count,yytext);}
{operator}      {count++;printf("%d\t(op,\'%s\')\n",count,yytext);}
{whitespace}    { /* do    nothing*/ }
{str}           {count++;printf("%d\t(string,\'%s\')\n",count,yytext);}
"("             {count++;printf("%d\t(left bracket,\'%s\')\n",count,yytext);}
")"             {count++;printf("%d\t(right bracket,\'%s\')\n",count,yytext);}
"{"             {count++;printf("%d\t(left bracket,\'%s\')\n",count,yytext);}
"}"             {count++;printf("%d\t(right bracket,\'%s\')\n",count,yytext);}
":"             {count++;printf("%d\t(colon,\'%s\')\n",count,yytext);}
";"             {count++;printf("%d\t(semicolon,\'%s\')\n",count,yytext);}
"#".*           {count++;printf("%d\t(head,\'%s\')\n",count,yytext);}
{other}		    {printf("illegal character:\'%s\'\n",yytext);}


%%
int main(){
	yyin=fopen("F:/HOMEWORK/Compiler/Lab2/test.c","r");
	yylex();
	return 0;
}
 int yywrap(){
 	return 1;
 }