%{
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include "node.h"
using namespace std;

extern int yylex();
extern int lineCount;
extern int indexCount;
extern char *yytext;
int status;
NExpr* root;



void yyerror ( const char* s ) 
{
	printf("%s in line %d at '%s'\n", s, lineCount, yytext);
	status = 0;
}

%}

%union {
	NExpr* exprType;
	NExpr* programType;
	NExprList* exprlistType;
	NExprList* exprseqType;
	NDecl* declType;
	NDeclList* decllistType;
	NVarDecl* vardeclType;
	NTypeDecl* typedeclType;
	NTypeDecl* typelistType;
	NFuncDecl* funcdeclType;
	NFuncDecl* funclistType;
	NType* typeType;
	NVar* varType;
	NFieldExprList* fieldlistType;
	NFieldTypeList* typefieldsType;
	Symbol* idType;
	int token;
}

%token <token> ID STRINGT
%token <token> INTEGERT
%token <token> ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF FUNCTION VAR TYPE ERROR COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE DOT PLUS MINUS TIMES DIVIDE EQ NEQ GT GE LT LE AND OR ASSIGN NIL BREAK

%type <exprType> expr
%type <programType> program
%type <exprlistType> exprlist
%type <exprseqType> exprseq
%type <declType> decl
%type <decllistType> decllist
%type <vardeclType> vardecl
%type <typedeclType> typedecl
%type <typelistType> typelist
%type <funcdeclType> funcdecl
%type <funclistType> funclist
%type <typeType> type
%type <varType> var
%type <fieldlistType> fieldlist
%type <typefieldsType> typefields
%type <idType> id

%right FUNCTION TYPE
%right OF
%right DO ELSE THEN
%nonassoc ASSIGN
%nonassoc EQ NEQ LT LE GT GE
%nonassoc UMINUS HIGHER_THAN_OP LOWER_THAN_ELSE
%left OR
%left AND
%left PLUS MINUS
%left TIMES DIVIDE
%left LPAREN


%%

program		: expr { root = $1; }
			;

expr	    : NIL { $$ = new NNilExpr(lineCount, indexCount); }
			| INTEGERT { $$ = new NIntExpr(lineCount, indexCount, atoi(yytext)); }
			| STRINGT { string s = yytext; $$ = new NStrExpr(lineCount, indexCount, s.substr(1, s.length()-2)); }
			| var { $$ = new NVarExpr(lineCount, indexCount, $1); }
			| MINUS expr %prec UMINUS { $$ = new NOpExpr(lineCount, indexCount, 0, 1, $2); }
			| expr PLUS expr { $$ = new NOpExpr(lineCount, indexCount, $1, 0, $3); }
			| expr MINUS expr { $$ = new NOpExpr(lineCount, indexCount, $1, 1, $3); }
			| expr TIMES expr { $$ = new NOpExpr(lineCount, indexCount, $1, 2, $3); }
			| expr DIVIDE expr { $$ = new NOpExpr(lineCount, indexCount, $1, 3, $3); }
			| expr AND expr { $$ = new NOpExpr(lineCount, indexCount, $1, 10, $3); }
			| expr OR expr { $$ = new NOpExpr(lineCount, indexCount, $1, 11, $3); }
			| expr EQ expr { $$ = new NOpExpr(lineCount, indexCount, $1, 4, $3); }
			| expr NEQ expr { $$ = new NOpExpr(lineCount, indexCount, $1, 5, $3); }
			| expr LT expr { $$ = new NOpExpr(lineCount, indexCount, $1, 6, $3); }
			| expr LE expr { $$ = new NOpExpr(lineCount, indexCount, $1, 7, $3); }
			| expr GT expr { $$ = new NOpExpr(lineCount, indexCount, $1, 8, $3); }
			| expr GE expr { $$ = new NOpExpr(lineCount, indexCount, $1, 9, $3); }
			| var ASSIGN expr { $$ = new NAssignExpr(lineCount, indexCount, $1, $3); }
			| id LPAREN exprlist RPAREN { $$ = new NCallExpr(lineCount, indexCount, $1, $3); }
			| id LPAREN RPAREN { $$ = new NCallExpr(lineCount, indexCount, $1, NULL); }
			| LPAREN exprseq RPAREN { $$ = new NSeqExpr(lineCount, indexCount, $2); }
			| id LBRACE fieldlist RBRACE { $$ = new NRecordExpr(lineCount, indexCount, $1, $3); } 
			| id LBRACE RBRACE { $$ = new NRecordExpr(lineCount, indexCount, $1, NULL); } 
			| id LBRACK expr RBRACK OF expr { $$ = new NArrayExpr(lineCount, indexCount, $1, $3, $6); }
			| IF expr THEN expr { $$ = new NIfExpr(lineCount, indexCount, $2, $4); }
			| IF expr THEN expr ELSE expr { $$ = new NIfExpr(lineCount, indexCount, $2, $4, $6); }
			| WHILE expr DO expr { $$ = new NWhileExpr(lineCount, indexCount, $2, $4); }
			| FOR id ASSIGN expr TO expr DO expr { $$ = new NForExpr(lineCount, indexCount, new NVarDecl(lineCount, indexCount, $2, $4, new NNameType(lineCount, indexCount, new Symbol("int"))), $6, $8); }
			| BREAK { $$ = new NBreakExpr(lineCount, indexCount); }
			| LET decllist IN END { $$ = new NLetExpr(lineCount, indexCount, $2, NULL); }
			| LET decllist IN exprseq END { $$ = new NLetExpr(lineCount, indexCount, $2, $4); } 
			| LPAREN RPAREN {  }
			;

id			: ID { $$ = new Symbol(string(yytext));}
			;

var			: id { $$ = new NSimpleVar(lineCount, indexCount, $1); }
			| var DOT id { $$ = new NFieldVar(lineCount, indexCount, $1, $3); }
			| var LBRACK expr RBRACK { $$ = new NSubscriptVar(lineCount, indexCount, $1, $3); }
			| id LBRACK expr RBRACK { $$ = new NSubscriptVar(lineCount, indexCount, new NSimpleVar(lineCount, indexCount, $1), $3); }
			;

exprlist	: expr { $$ = new NExprList(lineCount, indexCount, $1, NULL); }
			| expr COMMA exprlist { $$ = new NExprList(lineCount, indexCount, $1, $3); }
			;

exprseq		: expr { $$ = new NExprList(lineCount, indexCount, $1, NULL); }
			| expr SEMICOLON exprseq { $$ = new NExprList(lineCount, indexCount, $1, $3); }
			;

fieldlist	: id EQ expr { $$ = new NFieldExprList(lineCount, indexCount, $1, $3, NULL); }
			| id EQ expr COMMA fieldlist { $$ = new NFieldExprList(lineCount, indexCount, $1, $3, $5); }
			;

decllist	: decl { $$ = new NDeclList(lineCount, indexCount, $1, NULL); }
			| decl decllist { $$ = new NDeclList(lineCount, indexCount, $1, $2); }
			;

decl		: typelist { $$ = $1; }
			| vardecl { $$ = $1; }
			| funclist { $$ = $1; }
			;

typelist	: typedecl { $$ = $1; }
			| typedecl typelist { $$ = new NTypeDecl(lineCount, indexCount, $1->id, $1->type, $2); }
			;

typedecl	: TYPE id EQ type { $$ = new NTypeDecl(lineCount, indexCount, $2, $4); }
			;

type		: id { $$ = new NNameType(lineCount, indexCount, $1); }
			| LBRACE typefields RBRACE { $$ = new NRecordType(lineCount, indexCount, $2); }
			| LBRACE RBRACE { $$ = new NRecordType(lineCount, indexCount, NULL); }
			| ARRAY OF id { $$ = new NArrayType(lineCount, indexCount, $3); }
			; 

typefields	: id COLON id { $$ = new NFieldTypeList(lineCount, indexCount, $1, $3, NULL); }
			| id COLON id COMMA typefields { $$ = new NFieldTypeList(lineCount, indexCount, $1, $3, $5); }
			;

vardecl		:  VAR id ASSIGN expr { $$ = new NVarDecl(lineCount, indexCount, $2, $4); }
			| VAR id COLON id ASSIGN expr { $$ = new NVarDecl(lineCount, indexCount, $2, $6, new NNameType(lineCount, indexCount, $4)); }
			;

funclist	: funcdecl { $$ = $1; }
			| funcdecl funclist { $$ = new NFuncDecl(lineCount, indexCount, $1->id, $1->params, $1->body, $1->retType, $2); }
			;

funcdecl	: FUNCTION id LPAREN RPAREN EQ expr { $$ = new NFuncDecl(lineCount, indexCount, $2, NULL, $6, NULL, NULL); }
			| FUNCTION id LPAREN typefields RPAREN EQ expr { $$ = new NFuncDecl(lineCount, indexCount, $2, $4, $7, NULL, NULL); }
			| FUNCTION id LPAREN RPAREN COLON id EQ expr { $$ = new NFuncDecl(lineCount, indexCount, $2, NULL, $8, new NNameType(lineCount, indexCount, $6), NULL); }
			| FUNCTION id LPAREN typefields RPAREN COLON id EQ expr { $$ = new NFuncDecl(lineCount, indexCount, $2, $4, $9, new NNameType(lineCount, indexCount, $7), NULL); }
			;


%%
