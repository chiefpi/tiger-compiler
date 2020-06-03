%{
#include <iostream>
#include <string>
#include <cstdlib>
#include "node.h"
#include "myparser.hpp"
using namespace std;

int lineCount;
int indexCount = 0;
int yycolumn = 1;
int commentCount = 0;

void yyerror ( const char* s );
%}

/* option为默认 */
%option noyywrap
/* 状态声明, 声明一个COMMENT状态来处理注释 */
%x COMMENT

%%
<*>"/*" { 
    // 进入COMMENT状态
    BEGIN(COMMENT);
    commentCount++;
    yymore(); 
}

<COMMENT>"*/" { 
    commentCount--;
    if (commentCount==0)
    {
        // 回到INITIAL状态
        BEGIN(INITIAL); 
    }
}

<COMMENT><<EOF>> { 
    yyerror("Unclosed comment.\n");
    return ERROR; 
}

<COMMENT>. {
    yymore();
}

<COMMENT>"\n\r"|"\r\n"|"\r"|"\n" {
    yycolumn = 1;
    lineCount++;
    yymore();
}

[ ]+  {
    printf("kongge\n");
}

<*>[\t] {
    printf("kongge\n");
}

"array"      return ARRAY;
"if"         return IF;
"then"       return THEN;
"else"       return ELSE;
"while"      return WHILE;
"for"        return FOR;
"to"         return TO;
"do"         return DO;
"let"        return LET;
"in"         return IN;
"end"        return END;
"of"         return OF;
"break"      return BREAK;
"nil"        return NIL;
"function"   return FUNCTION;
"var"        return VAR;
"type"       return TYPE;

","	    return COMMA;
":"	    return COLON;
";"	    return SEMICOLON;
"("	    return LPAREN;
")"	    return RPAREN;
"["	    return LBRACK;
"]"	    return RBRACK;
"{"	    return LBRACE;
"}"	    return RBRACE;
"."	    return DOT;
"+"	    return PLUS;
"-"	    return MINUS;
"*"	    return TIMES;
"/"	    return DIVIDE;
"="	    return EQ;
"<>"    return NEQ;
"<"	    return LT;
"<="    return LE;
">"	    return GT;
">="    return GE;
"&"     return AND;
"|"     return OR;
":="    return ASSIGN;

<*>"\n\r"|"\r\n"|"\r"|"\n" {
    yycolumn = 1;
    lineCount++;
}

\"[^\"]*\" {
    // 是否超出长度
    if(yyleng > 255)
    {
        yyerror("String too long");
        return ERROR;
    }
    int length;
    length = yyleng - 1;
    for (int i = 1; i < length; i++)
    {
        // 转义字符
        if (yytext[i] == '\\')
        {
            // 八进制表示转化
            if (i+3 < length && yytext[i+1] >= '0' && yytext[i+1] <= '3' && yytext[i+2] >= '0' && yytext[i+2] <= '7' && yytext[i+3] >= '0' && yytext[i+3] <= '7')
            {
                yytext[i] = 64*(yytext[i+1]-'0') + 8*(yytext[i+2]-'0') + (yytext[i+3]-'0');
                for (int j = i+1; j < length-2; j++)
                {
                    yytext[j] = yytext[j+3];
                }
                yytext[length-2] = 0;
                yytext[length-1] = 0;
                yytext[length] = 0;
                length -= 3;
            }
            // 十六进制表示转化
            else if (i+3 < length && yytext[i+1]=='x' && ((yytext[i+2] >= '0' && yytext[i+2] <= '9') || (yytext[i+2] >= 'A' && yytext[i+2] <= 'F') || (yytext[i+2] >= 'a' && yytext[i+2] <= 'f')) && ((yytext[i+3] >= '0' && yytext[i+3] <= '9') || (yytext[i+3] >= 'A' && yytext[i+3] <= 'F') || (yytext[i+3] >= 'a' && yytext[i+3] <= 'f')))
            {
                yytext[i] = 0;
                if (yytext[i+2] >= '0' && yytext[i+2] <= '9')
                {
                    yytext[i] += 16*(yytext[i+2] - '0');
                }
                else if (yytext[i+2] >= 'A' && yytext[i+2] <= 'F')
                {
                    yytext[i] += 16*(yytext[i+2] - 'A');
                }
                else if (yytext[i+2] >= 'a' && yytext[i+2] <= 'f')
                {
                    yytext[i] += 16*(yytext[i+2] - 'a');
                }
                if (yytext[i+3] >= '0' && yytext[i+3] <= '9')
                {
                    yytext[i] += (yytext[i+3] - '0');
                }
                else if (yytext[i+3] >= 'A' && yytext[i+3] <= 'F')
                {
                    yytext[i] += (yytext[i+3] - 'A');
                }
                else if (yytext[i+3] >= 'a' && yytext[i+3] <= 'f')
                {
                    yytext[i] += (yytext[i+3] - 'a');
                }
                for (int j = i+1; j < length-2; j++)
                {
                    yytext[j] = yytext[j+3];
                }
                yytext[length-2] = 0;
                yytext[length-1] = 0;
                yytext[length] = 0;
                length -= 3;
            }
            // 其他转义字符
            else if (i+1 < length && (yytext[i+1]=='a' || yytext[i+1]=='b' || yytext[i+1]=='f' || yytext[i+1]=='n' || yytext[i+1]=='r' || yytext[i+1]=='t' || yytext[i+1]=='v' || yytext[i+1]=='\\' || yytext[i+1]=='"' ))
            {
                switch (yytext[i+1])
                {
                    case 'a': yytext[i]=7; break;
                    case 'b': yytext[i]=8; break;
                    case 'f': yytext[i]=12; break;
                    case 'n': yytext[i]=10; break;
                    case 'r': yytext[i]=13; break;
                    case 't': yytext[i]=9; break;
                    case '\\': yytext[i]=92; break;
                    case '"': yytext[i]=34; break;
                }
                for ( int j = i+1; j < length; j++)
                {
                    yytext[j] = yytext[j+1];
                }
                yytext[length] = 0;
                length -= 1;
            }
            else
            {
                yyerror("Invalid");
            }
        }
    }
    return STRINGT;
}

\"[^\"]* {
    yyerror("Unclosed string");
    return 0;
}

[a-zA-Z][a-zA-Z_0-9]*|"_main" {
    yyerror("cao");
    if(yyleng>255)
    {
        yyerror("Identifier too long");
        return ERROR;
    }
    return ID;
}

[0-9]+ {
    if(yyleng>10||(yyleng==10&&(strcmp(yytext,"2147483647")>0)))
    {
        yyerror("Out of range");
        return ERROR;                  
    } 
    return INTEGERT;
}

<<EOF>> return 0;

. {
    yyerror("Unknown token");
}

%%
