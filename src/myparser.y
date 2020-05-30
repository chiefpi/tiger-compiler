%{
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include "node.h"
using namespace std;

extern int lineCount;
extern int index;
extern char *yytext;
extern int status;
extern NExpr* root;

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
	NFieldExpr* fieldType;
	NFieldExprList* fieldlistType;
	NFieldTypeList* typefieldsType;
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
%type <fieldType> field
%type <fieldlistType> fieldlist
%type <typefieldsType> typefields

%nonassoc ASSIGN EQ NEQ LT LE GT GE UMINUS HIGHER_THAN_OP LOWER_THAN_ELSE
%left OR AND PLUS MINUS TIMES DIVIDE DOT LPAREN
%right FUNCTION TYPE OF DO ELSE THEN

%%

program		: expr { root = new NExpr(); }
			;

expr	    : NIL { $$ = new NNilExpr(lineCount, index); }
			| INTEGERT { $$ = new NIntExpr(lineCount, index, atoi(yytext)); }
			| STRINGT { string s = yytext; $$ = new NStrExpr(lineCount, index, s.substr(1, s.length()-2)); }
			| var { $$ = $1; }
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
			| ID LPAREN exprlist RPAREN { $$ = new NCallExpr(lineCount, index, Symbol.symbol($1), $3); }
			| LPAREN exprseq RPAREN { $$ = new NExprList(lineCount, index, $2, NULL); }
			| ID LBRACE fieldlist RBRACE { $$ = new NRecordExpr(lineCount, index, Symbol.symbol($1), $3); } 
			| ID LBRACE RBRACE { $$ = new NRecordExpr(lineCount, index, Symbol.symbol($1), NULL); } 
			| ID LBRACK expr RBRACK OF expr { $$ = new NArrayExpr(lineCount, index, Symbol.symbol($1), $3, $6); }
			| IF expr THEN expr { $$ = new NIfExpr(lineCount, index, $2, $4); }
			| IF expr THEN expr ELSE expr { $$ = new NIfExpr(lineCount, index, $2, $4, $6); }
			| WHILE expr DO expr { $$ = new NWhileExpr(lineCount, index, $2, $4); }
			| FOR ID ASSIGN expr TO expr DO expr { $$ = new NForExpr(lineCount, index, NNameType(lineCount, index, Symbol.symbol($2)), $4, $6, $8); }
			| BREAK { $$ = new NBreakExpr(lineCount, index); }
			| LET decllist IN END { $$ = new NLetExpr(lineCount, index, $2, NULL); }
			| LET decllist IN exprseq END { $$ = new NLetExpr(lineCount, index, $2, $4); } 
			;

var			: ID { $$ = new NSimpleVar(lineCount, index, Symbol.symbol($1)); }
			| var DOT ID { $$ = new NFieldVar(lineCount, index, Symbol.symbol($1)); }
			| var LBRACK expr RBRACK { $$ = new NSubscriptVar(lineCount, index, $1, $3); }
			| ID LBRACK expr RBRACK { $$ = new NSubscriptVar(lineCount, index, $1, $3); }
			;

exprlist	: expr { $$ = new NExprList(lineCount, index, $1, NULL); }
			| expr COMMA exprlist { $$ = new NExprList(lineCount, index, $1, $3); }
			;

exprseq		: expr { $$ = new NExprList(lineCount, index, $1, NULL); }
			| expr SEMICOLON exprlist { $$ = new NExprList(lineCount, index, $1, $3); }
			;

fieldlist	: ID EQ expr { $$ = new NFieldExprList(lineCount, index, Symbol.symbol($1), $3, NULL); }
			| ID EQ expr COMMA fieldlist { $$ = new NFieldExprList(lineCount, index, Symbol.symbol($1), $3, $5); }
			;

decllist	: decl { $$ = new NDeclList(lineCount, index, $1, NULL); }
			| decl decllist { $$ = new NDeclList(lineCount, index, $1, $2); }
			;

decl		: typelist { $$ = $1; }
			| vardecl { $$ = $1; }
			| funclist { $$ = $1; }
			;

typelist	: typedecl { $$ = $1; }
			| typedecl typelist { $$ = new NTypeDecl(lineCount, index, Symbol.symbol($1), $2); }
			;

typedecl	: TYPE ID EQ type { $$ = NTypeDecl(lineCount, index, Symbol.symbol($1), $2); }
			;

type		: ID { $$ = NNameType(lineCount, index, Symbol.symbol($1)); }
			| LBRACE typefields RBRACE { $$ = new NRecordType(lineCount, index, $2); }
			| LBRACE RBRACE { $$ = new NRecordType(lineCount, index, NULL); }
			| ARRAY OF ID { $$ = new NArrayType(lineCount, index, Symbol.symbol($3)); }
			; 

typefields	: ID COLON ID { $$ = new NFieldTypeList(lineCount, index, Symbol.symbol($1), Symbol.symbol($3), null); }
			| ID COLON ID COMMA typefields { $$ = new NFieldList(lineCount, index, Symbol.symbol($1), Symbol.symbol($3), $5); }
			;

vardecl		:  VAR ID ASSIGN expr { $$ = new NVarDecl(lineCount, index, Symbol.symbol($2), $4); }
			| VAR ID COLON ID ASSIGN expr { $$ = new NVarDecl(lineCount, index, Symbol.symbol($2), NNameType(lineCount, index, Symbol.symbol($4)), $6); }
			;

funclist	: funcdecl { $$ = $1; }
			| funcdecl funclist { $$ = new NFuncDecl(lineCount, index, NULL, NULL, NULL, NULL, $2); }
			;

funcdecl	: FUNCTION ID LPAREN RPAREN EQ expr { $$ = new NFuncDecl(lineCount, index, Symbol.symbol($2), NULL, NULL, $6, NULL); }
			| FUNCTION ID LPAREN typefields RPAREN EQ expr { $$ = nnew NFuncDecl(lineCount, index, Symbol.symbol($2), $4, NULL, $7, NULL); }
			| FUNCTION ID LPAREN RPAREN COLON ID EQ expr { $$ = new NFuncDecl(lineCount, index, Symbol.symbol($2), NULL, NNameType(lineCount, index, Symbol.symbol($6)), $8, NULL); }
			| FUNCTION ID LPAREN typefields RPAREN COLON ID EQ expr { $$ = new NFuncDecl(lineCount, index, Symbol.symbol($2), $4, NNameType(lineCount, index, Symbol.symbol($7)), $9, NULL); }
			;


%%
