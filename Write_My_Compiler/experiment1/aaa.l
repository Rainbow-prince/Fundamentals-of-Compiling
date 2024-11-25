%{
	#include <stdio.h>
	#include <stdlib.h>
	int count = 0;
    char commentBuffer[1024] = {0}; // 用于暂存块注释内容
%}

%x COMMENT

digit		    [0-9]
letter		    [a-zA-Z]
reservedWord	while|if|else|switch|case|int|main|using|namespace|std|printf
id	            ({letter}|_)({letter}|{digit}|_)*
num	            {digit}+
operator	    \_|\+|\+\+|\+=|-|--|-=|->|\*|\*=|\/|\/=|\%|\%=|<|<=|>|>=|==|\!=|=|;|,|\(|\)|\[|\]|\{|\}|\/\/.*
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

"/*"                    { BEGIN(COMMENT); count++; commentBuffer[0] = '\0'; }
<COMMENT>[^*\n]*        { strncat(commentBuffer, yytext, sizeof(commentBuffer) - strlen(commentBuffer) - 1); }
<COMMENT>"*"+[^/\n]*    { strncat(commentBuffer, yytext, sizeof(commentBuffer) - strlen(commentBuffer) - 1); }
<COMMENT>\n             { strncat(commentBuffer, yytext, sizeof(commentBuffer) - strlen(commentBuffer) - 1); }
<COMMENT>"*"+"/"        { BEGIN(INITIAL); printf("%d\t(特殊符号,\'/*%s*/\')\n",count,commentBuffer); }

%%
int main(){
	yyin=fopen("test.cpp","r");
	printf("【扫描结果】：\n");
	yylex();
	return 0;
}
int yywrap(){
 	return 1;
}
