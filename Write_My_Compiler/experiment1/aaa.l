%{
	#include <stdio.h>
	#include <stdlib.h>
	int count = 0;

%}
digit		    [0-9]
letter		    [a-zA-Z]
reservedWord	while|if|else|switch|case|int|main|using|namespace|std|printf
id	            ({letter}|_)({letter}|{digit}|_)*
num	            {digit}+
operator	    \_|\+|\+\+|\+=|-|--|-=|->|\*|\*=|\/|\/=|\%|\%=|<|<=|>|>=|==|\!=|=|;|,|\(|\)|\[|\]|\{|\}|\/\/.*|\/\*[^*]*\*+(?:[^/*][^*]*\*+)*\/
delimiter		[ \t\n\r]
whitespace	    {delimiter}+
semicolon       [\;]
str             \"[^"]*\"
other		    .


%%
{reservedWord}  {count++;printf("%d\t(关键字,\'%s\')\n",count,yytext);}
{id}            {count++;printf("%d\t(标识符,\'%s\')\n",count,yytext);}
{num}	        {count++;printf("%d\t(数字,\'%s\')\n",count,yytext);}
{operator}      {count++;printf("%d\t(特殊符号,\'%s\')\n",count,yytext);}
{whitespace}    { /* do    nothing*/ }
{str}           {count++;printf("%d\t(字符串,\'%s\')\n",count,yytext);}
":"             {count++;printf("%d\t(colon,\'%s\')\n",count,yytext);}
"#".*           {count++;printf("%d\t(特殊符号,\'%s\')\n",count,yytext);}
{other}		    {printf("illegal character:\'%s\'\n",yytext);}


%%
int main(){
	yyin=fopen("test.cpp","r");
	yylex();
	return 0;
}
 int yywrap(){
 	return 1;
 }
