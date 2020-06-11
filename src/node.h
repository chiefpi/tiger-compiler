#pragma once

#include "env.h"
#include "semant.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <llvm/IR/Value.h>

using namespace std;

class CodeGenContext;

class Node;
class NArrayExpr;
class NArrayType;
class NAssignExpr;
class NBreakExpr;
class NCallExpr;
class NDecl;
class NDeclList;
class NExpr;
class NExprList;
class NFieldExprList;
class NFieldTypeList;
class NFieldVar;
class NForExpr;
class NFuncDecl;
class NIfExpr;
class NIntExpr;
class NLetExpr;
class NNameType;
class NNilExpr;
class NOpExpr;
class NRecordExpr;
class NRecordType;
class NSeqExpr;
class NSimpleVar;
class NStrExpr;
class NSubscriptVar;
class NType;
class NTypeDecl;
class NVar;
class NVarExpr;
class NVarDecl;
class NVarList;
class NWhileExpr;

class Node
{
public:
	static ofstream os;
	int line;
	int index;
	virtual ~Node() {}
	virtual llvm::Value *codeGen(CodeGenContext &context) { return NULL; }
	virtual void print(int depth = 0) const = 0;
	virtual Type *traverse(Semant *analyzer) = 0;
	void reportErr(string msg)
	{
		fprintf(stderr, "In line %d: %s\n", line, msg.c_str());
		exit(1); // TODO: check when to exit
	}
	void printIndent(int d) const
	{
		for (int i = 0; i < d; i++)
			cout << "  ";
	}
	bool assertpred(bool pred, string msg)
	{
		if (!pred)
		{
			reportErr(msg);
		}
		return pred;
	}
};

class NExpr : public Node
{
};

class NExprList : public Node
{
public:
	NExpr *head;
	NExprList *next;

	NExprList(int line, int index, NExpr *head, NExprList *next)
		: head(head), next(next)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NDecl : public Node
{
};

class NDeclList : public Node
{
public:
	NDecl *head;
	NDeclList *next;

	NDeclList(int line, int index, NDecl *head, NDeclList *next)
		: head(head), next(next)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NVar : public Node
{
};

class NVarList : public Node
{
public:
	NVar *head;
	NVarList *next;

	NVarList(int line, int index, NVar *head, NVarList *next)
		: head(head), next(next)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NType : public Node
{
};

class NFieldTypeList : public Node
{
public:
	Symbol *id;
	Symbol *type;
	NFieldTypeList *next;
	NFieldTypeList(int line, int index, Symbol *id, Symbol *type, NFieldTypeList *next)
		: id(id), type(type), next(next)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NFieldExprList : public Node
{
public:
	Symbol *id;
	NExpr *initValue;
	NFieldExprList *next;
	NFieldExprList(int line, int index, Symbol *id, NExpr *initValue, NFieldExprList *next)
		: id(id), initValue(initValue), next(next)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

/* 	Expressions	 */

class NStrExpr : public NExpr
{
public:
	string value;
	NStrExpr(int line, int index, string val)
		: value(val)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NIntExpr : public NExpr
{
public:
	long long value;
	NIntExpr(int line, int index, long long val)
		: value(val)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NNilExpr : public NExpr
{
public:
	NNilExpr(int line, int index)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NVarExpr : public NExpr
{
public:
	NVar *var;
	NVarExpr(int line, int index, NVar *var) : var(var)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NOpExpr : public NExpr
{
public:
	NExpr *lhs;
	int op;
	NExpr *rhs;

	NOpExpr(int line, int index, NExpr *lhs, int op, NExpr *rhs)
		: lhs(lhs), op(op), rhs(rhs)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);

	const static int PLUS = 0, MINUS = 1, MUL = 2, DIV = 3, EQ = 4, NE = 5, LT = 6, LE = 7, GT = 8, GE = 9, AND = 10, OR = 11;
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NAssignExpr : public NExpr
{
public:
	NVar *var;
	NExpr *rhs;

	NAssignExpr(int line, int index, NVar *var, NExpr *rhs)
		: var(var), rhs(rhs)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NRecordExpr : public NExpr
{
public:
	Symbol *type;
	NFieldExprList *fields;

	NRecordExpr(int line, int index, Symbol *type, NFieldExprList *fields)
		: type(type), fields(fields)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NArrayExpr : public NExpr
{
public:
	Symbol *type;
	NExpr *size;
	NExpr *initValue;

	NArrayExpr(int line, int index, Symbol *type, NExpr *size, NExpr *initValue)
		: type(type), size(size), initValue(initValue)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NCallExpr : public NExpr
{
public:
	Symbol *func;
	NExprList *args; // opt

	NCallExpr(int line, int index, Symbol *func, NExprList *args)
		: func(func), args(args)
	{
		this->line = line;
		this->index = index;
	}
	NCallExpr(int line, int index, Symbol *func)
		: func(func)
	{
		this->line = line;
		this->index = index;
		args = NULL;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NSeqExpr : public NExpr
{
public:
	NExprList *exprs;

	NSeqExpr(int line, int index, NExprList *exprs)
		: exprs(exprs)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NIfExpr : public NExpr
{
public:
	NExpr *test;
	NExpr *thenClause;
	NExpr *elseClause; // opt

	NIfExpr(int line, int index, NExpr *test, NExpr *thencls, NExpr *elsecls)
		: test(test), thenClause(thencls), elseClause(elsecls)
	{
		this->line = line;
		this->index = index;
	}
	NIfExpr(int line, int index, NExpr *test, NExpr *thencls)
		: test(test), thenClause(thencls)
	{
		this->line = line;
		this->index = index;
		elseClause = NULL;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NWhileExpr : public NExpr
{
public:
	NExpr *test;
	NExpr *body;

	NWhileExpr(int line, int index, NExpr *test, NExpr *body)
		: test(test), body(body)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NForExpr : public NExpr
{
public:
	NVarDecl *id;
	NExpr *high;
	NExpr *body;

	NForExpr(int line, int index, NVarDecl *id, NExpr *high, NExpr *body)
		: id(id), high(high), body(body)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NBreakExpr : public NExpr
{
public:
	NBreakExpr(int line, int index)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NLetExpr : public NExpr
{
public:
	NDeclList *decls;
	NExprList *body; // Expr -> ExprList

	NLetExpr(int line, int index, NDeclList *decls, NExprList *body)
		: decls(decls), body(body)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

/* Declarations */

class NFuncDecl : public NDecl
{
public:
	Symbol *id;
	NFieldTypeList *params;
	NExpr *body;
	NNameType *retType; // opt
	NFuncDecl *next;	// opt

	NFuncDecl(int line, int index, Symbol *id, NFieldTypeList *params, NExpr *body, NNameType *retType, NFuncDecl *next)
		: id(id), params(params), body(body), retType(retType), next(next)
	{
		this->line = line;
		this->index = index;
	}
	NFuncDecl(int line, int index, Symbol *id, NFieldTypeList *params, NExpr *body, NNameType *retType)
		: id(id), params(params), body(body), retType(retType)
	{
		this->line = line;
		this->index = index;
		next = NULL;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
	virtual Type *traverse(Semant *analyzer, bool notHead);
};

class NTypeDecl : public NDecl
{
public:
	Symbol *id;
	NType *type;
	NTypeDecl *next; // opt

	NTypeDecl(int line, int index, Symbol *id, NType *type, NTypeDecl *next)
		: id(id), type(type), next(next)
	{
		this->line = line;
		this->index = index;
	}
	NTypeDecl(int line, int index, Symbol *id, NType *type)
		: id(id), type(type)
	{
		this->line = line;
		this->index = index;
		next = NULL;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
	virtual Type *traverse(Semant *analyzer, bool notHead);
};

class NVarDecl : public NDecl
{
public:
	Symbol *id;
	NExpr *initValue;
	NType *type; // opt
	// boolean escape = true;

	NVarDecl(int line, int index, Symbol *id, NExpr *initValue, NType *type)
		: id(id), initValue(initValue), type(type)
	{
		this->line = line;
		this->index = index;
	}
	NVarDecl(int line, int index, Symbol *id, NExpr *initValue)
		: id(id), initValue(initValue)
	{
		this->line = line;
		this->index = index;
		type = NULL;
	}

	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

/* Types */

class NArrayType : public NType
{
public:
	Symbol *id;

	NArrayType(int line, int index, Symbol *id)
		: id(id)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NRecordType : public NType
{
public:
	NFieldTypeList *fields;

	NRecordType(int line, int index, NFieldTypeList *fields)
		: fields(fields)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NNameType : public NType
{
public:
	Symbol *id;

	NNameType(int line, int index, Symbol *id)
		: id(id)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

/* Variables */

class NSimpleVar : public NVar
{
public:
	Symbol *id;

	NSimpleVar(int line, int index, Symbol *id)
		: id(id)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NFieldVar : public NVar
{
public:
	NVar *var;
	Symbol *id;

	NFieldVar(int line, int index, NVar *var, Symbol *id)
		: var(var), id(id)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};

class NSubscriptVar : public NVar
{
public:
	NVar *var;
	NExpr *sub;

	NSubscriptVar(int line, int index, NVar *var, NExpr *sub)
		: var(var), sub(sub)
	{
		this->line = line;
		this->index = index;
	}
	virtual llvm::Value *codeGen(CodeGenContext &context);
	virtual void print(int depth) const;
	virtual Type *traverse(Semant *analyzer);
};
