%{
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include "node.h"
using namespace std;

extern int yylex();
extern int lineCount;
extern int index;
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

%token <token> ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF FUNCTION VAR TYPE ERROR COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE DOT PLUS MINUS STAR SLASH EQ NEQ GT GE LT LE AND OR ASSIGN ID STRINGT INTEGERT NIL BREAK

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

%nonassoc ASSIGN EQ NEQ LT LE GT GE UMINUS HIGHER_THAN_OP LOWER_THAN_ELSE
%left OR AND PLUS MINUS TIMES DIVIDE DOT LPAREN
%right FUNCTION TYPE OF DO ELSE THEN

%%

program		: expr { root = $1; }
			;

expr	    : NIL { $$ = new NNilExpr(lineCount, index); }
			| INTEGERT { $$ = new NIntExpr(lineCount, index, atoi(yytext)); }
			| STRINGT { string s = yytext; $$ = new NStrExpr(lineCount, index, s.substr(1, s.length()-2)); }
			| var { $$ = new NVarExpr(lineCount, index, $1); }
			| MINUS expr %prec UMINUS { $$ = new NOpExpr(lineCount, index, 0, 1, $2); }
			| expr PLUS expr { $$ = new NOpExpr(lineCount, index, $1, 0, $3); }
			| expr MINUS expr { $$ = new NOpExpr(lineCount, index, $1, 1, $3); }
			| expr STAR expr { $$ = new NOpExpr(lineCount, index, $1, 2, $3); }
			| expr SLASH expr { $$ = new NOpExpr(lineCount, index, $1, 3, $3); }
			| expr EQ expr { $$ = new NOpExpr(lineCount, index, $1, 4, $3); }
			| expr NEQ expr { $$ = new NOpExpr(lineCount, index, $1, 5, $3); }
			| expr LT expr { $$ = new NOpExpr(lineCount, index, $1, 6, $3); }
			| expr LE expr { $$ = new NOpExpr(lineCount, index, $1, 7, $3); }
			| expr GT expr { $$ = new NOpExpr(lineCount, index, $1, 8, $3); }
			| expr GE expr { $$ = new NOpExpr(lineCount, index, $1, 9, $3); }
			| var ASSIGN expr { $$ = new NAssignExpr(lineCount, index, $1, $3); }
			| id LPAREN exprlist RPAREN { $$ = new NCallExpr(lineCount, index, $1, $3); }
			| LPAREN exprseq RPAREN { $$ = new NSeqExpr(lineCount, index, $2); }
			| id LBRACE fieldlist RBRACE { $$ = new NRecordExpr(lineCount, index, $1, $3); } 
			| id LBRACE RBRACE { $$ = new NRecordExpr(lineCount, index, $1, NULL); } 
			| id LBRACK expr RBRACK OF expr { $$ = new NArrayExpr(lineCount, index, $1, $3, $6); }
			| IF expr THEN expr { $$ = new NIfExpr(lineCount, index, $2, $4); }
			| IF expr THEN expr ELSE expr { $$ = new NIfExpr(lineCount, index, $2, $4, $6); }
			| WHILE expr DO expr { $$ = new NWhileExpr(lineCount, index, $2, $4); }
			| FOR id ASSIGN expr TO expr DO expr { $$ = new NForExpr(lineCount, index, new NVarDecl(lineCount, index, $2, $4, new NNameType(lineCount, index, new Symbol("int"))), $6, $8); }
			| BREAK { $$ = new NBreakExpr(lineCount, index); }
			| LET decllist IN END { $$ = new NLetExpr(lineCount, index, $2, NULL); }
			| LET decllist IN exprseq END { $$ = new NLetExpr(lineCount, index, $2, $4); } 
			;

id			: ID { $$ = new Symbol(string(yytext));}
			;

var			: id { $$ = new NSimpleVar(lineCount, index, $1); }
			| var DOT id { $$ = new NFieldVar(lineCount, index, $1, $3); }
			| var LBRACK expr RBRACK { $$ = new NSubscriptVar(lineCount, index, $1, $3); }
			| id LBRACK expr RBRACK { $$ = new NSubscriptVar(lineCount, index, new NSimpleVar(lineCount, index, $1), $3); }
			;

exprlist	: expr { $$ = new NExprList(lineCount, index, $1, NULL); }
			| expr COMMA exprlist { $$ = new NExprList(lineCount, index, $1, $3); }
			;

exprseq		: expr { $$ = new NExprList(lineCount, index, $1, NULL); }
			| expr SEMICOLON exprlist { $$ = new NExprList(lineCount, index, $1, $3); }
			;

fieldlist	: id EQ expr { $$ = new NFieldExprList(lineCount, index, $1, $3, NULL); }
			| id EQ expr COMMA fieldlist { $$ = new NFieldExprList(lineCount, index, $1, $3, $5); }
			;

decllist	: decl { $$ = new NDeclList(lineCount, index, $1, NULL); }
			| decl decllist { $$ = new NDeclList(lineCount, index, $1, $2); }
			;

decl		: typelist { $$ = $1; }
			| vardecl { $$ = $1; }
			| funclist { $$ = $1; }
			;

typelist	: typedecl { $$ = $1; }
			| typedecl typelist { $$ = new NTypeDecl(lineCount, index, $1->id, $1->type, $2); }
			;

typedecl	: TYPE id EQ type { $$ = new NTypeDecl(lineCount, index, $2, $4); }
			;

type		: id { $$ = new NNameType(lineCount, index, $1); }
			| LBRACE typefields RBRACE { $$ = new NRecordType(lineCount, index, $2); }
			| LBRACE RBRACE { $$ = new NRecordType(lineCount, index, NULL); }
			| ARRAY OF id { $$ = new NArrayType(lineCount, index, $3); }
			; 

typefields	: id COLON id { $$ = new NFieldTypeList(lineCount, index, $1, $3, NULL); }
			| id COLON id COMMA typefields { $$ = new NFieldTypeList(lineCount, index, $1, $3, $5); }
			;

vardecl		:  VAR id ASSIGN expr { $$ = new NVarDecl(lineCount, index, $2, $4); }
			| VAR id COLON id ASSIGN expr { $$ = new NVarDecl(lineCount, index, $2, $6, new NNameType(lineCount, index, $4)); }
			;

funclist	: funcdecl { $$ = $1; }
			| funcdecl funclist { $$ = new NFuncDecl(lineCount, index, NULL, NULL, NULL, NULL, $2); }
			;

funcdecl	: FUNCTION id LPAREN RPAREN EQ expr { $$ = new NFuncDecl(lineCount, index, $2, NULL, $6, NULL, NULL); }
			| FUNCTION id LPAREN typefields RPAREN EQ expr { $$ = new NFuncDecl(lineCount, index, $2, $4, $7, NULL, NULL); }
			| FUNCTION id LPAREN RPAREN COLON id EQ expr { $$ = new NFuncDecl(lineCount, index, $2, NULL, $8, new NNameType(lineCount, index, $6), NULL); }
			| FUNCTION id LPAREN typefields RPAREN COLON id EQ expr { $$ = new NFuncDecl(lineCount, index, $2, $4, $9, new NNameType(lineCount, index, $7), NULL); }
			;


%%
