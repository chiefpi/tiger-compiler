#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>
#include "symbol.h"

class CodeGenContext;

class Node
{
public:
	int line;
	int index;
	virtual ~Node() {}
	virtual llvm::Value *codeGen(CodeGenContext &context) { return NULL; }
};

class NExpr : public Node
{
};

class NExprList : public Node
{
public:
	NExpr &head;
	NExprList &next;

	NExprList(int line, int index, NExpr &head, NExpr &next)
		: line(line), index(index), head(head), next(next) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NDecl : public Node
{
};

class NDeclList : public Node
{
public:
	NDecl &head;
	NDeclList &next;

	NDeclList(int line, int index, NExpr &head, NExpr &next)
		: line(line), index(index), head(head), next(next) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NVar : public Node
{
};

class NVarList : public Node
{
public:
	NVar &head;
	NVarList &next;

	NVarList(int line, int index, NExpr &head, NExpr &next)
		: line(line), index(index), head(head), next(next) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NType : public Node
{
};

class NFieldTypeList : public Node
{
public:
	Symbol &id;
	Symbol &type;
	NFieldTypeList &next;
	NFieldTypeList(int line, int index, Symbol &id, Symbol &type, NFieldTypeList &next)
		: line(line), index(index), id(id), type(type), next(next) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NFieldExprList : public Node
{
public:
	Symbol &id;
	NExpr &initValue;
	NFieldExprList &next;
	NFieldExprList(int line, int index, Symbol &id, NExpr &initValue, NFieldExprList &next)
		: line(line), index(index), id(id), initValue(initValue), next(next) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

/* 	Expressions	 */

class NStrExpr : public NExpr
{
public:
	string value;
	NStrExpr(int line, int index, string val)
		: line(line), index(index), value(val) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NIntExpr : public NExpr
{
public:
	long long value;
	NIntExpr(int line, int index, long long val)
		: line(line), index(index), value(val) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NNilExpr : public NExpr
{
public:
	NNilExpr(int line, int index) : line(line), index(index) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NOpExpr : public NExpr
{
public:
	NExpr &lhs;
	NExpr &rhs;
	int op;

	NOpExpr(int line, int index, NExpr &lhs, int op, NExpr &rhs)
		: line(line), index(index), lhs(lhs), op(op), rhs(rhs) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);

	const static int PLUS = 0, MINUS = 1, MUL = 2, DIV = 3, EQ = 4, NE = 5, LT = 6, LE = 7, GT = 8, GE = 9;
};

class NAssignExpr : public NExpr
{
public:
	NVar &var;
	NExpr &rhs;

	NAssignExpr(int line, int index, NVar &var, NExpr &rhs)
		: line(line), index(index), var(var), rhs(rhs) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NRecordExpr : public NExpr
{
public:
	Symbol &type;
	NFieldExprList &fields;

	NRecordExpr(int line, int index, Symbol &type, NFieldExprList &fields)
		: line(line), index(index), type(type), fields(fields) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NArrayExpr : public NExpr
{
public:
	Symbol &type;
	NExpr &size;
	NExpr &initValue;

	NArrayExpr(int line, int index, Symbol &type, NExpr &size, NExpr &initValue)
		: line(line), index(index), type(type), size(size), initValue(initValue) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NCallExpr : public NExpr
{
public:
	const Symbol &func;
	NExprList &args; // opt

	NCallExpr(int line, int index, cosnt Symbol &func, NExprList &args)
		: line(line), index(index), func(func), args(args) {}
	NCallExpr(int line, int index, cosnt Symbol &func)
		: line(line), index(index), func(func) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NSeqExpr : public NExpr
{
public:
	NExprlist &exprs;

	NSeqExpr(int line, int index, NExprList &exprs)
		: line(line), index(index), exprs(exprs) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NIfExpr : public NExpr
{
public:
	NExpr &test;
	NExpr &thenClause;
	NExpr &elseClause;

	NIfExpr(int line, int index, NExpr &test, NExpr &thencls, NExpr &elsecls)
		: line(line), index(index), test(test), thenClause(thencls), elseClause(elsecls) {}
	NIfExpr(int line, int index, NExpr &test, NExpr &thencls)
		: line(line), index(index), test(test), thenClause(thencls) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NWhileExpr : public NExpr
{
public:
	NExpr &test;
	NExpr &body;

	NWhileExpr(int line, int index, NExpr &test, NExpr &body)
		: line(line), index(index), test(test), body(body) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NForExpr : public NExpr
{
public:
	NVarDecl &id;
	NExpr &high;
	NExpr &body;

	NForExpr(int line, int index, NVarDecl &id, NExpr &high, NExpr &body)
		: line(line), index(index), id(id), high(high), body(body) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NBreakExpr : public NExpr
{
public:
	NBreakExpr(int line, int index)
		: line(line), index(index) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NLetExpr : public NExpr
{
public:
	NDeclList &decls;
	NExpr &body;

	NLetExpr(int line, int index, NDeclList &decls, NExpr &body)
		: line(line), index(index), decls(decls), body(body) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
};

/* Declarations */

class NFuncDecl : public NDecl
{
public:
	Symbol &id;
	NFieldTypeList &params;
	NExpr &body;
	NNameType &retType; // opt
	NFuncDecl &next;	// opt

	NFuncDecl(int line, int index, Symbol &id, NFieldTypeList &params, NExpr &body, NNameType &retType, NFuncDecl &next)
		: line(line), index(index), id(id), params(params), body(body), retType(retType), next(next) {}
	NFuncDecl(int line, int index, Symbol &id, NFieldTypeList &params, NExpr &body, NNameType &retType)
		: line(line), index(index), id(id), params(params), body(body), retType(retType) { next = NULL; }
	virtual llvm::Value *codeGen(CodeGenContext &context);
}

class NTypeDecl : public NDecl
{
public:
	Symbol &id;
	NType &type;
	NTypeDecl &next;

	NTypeDecl(int line, int index, Symbol &id, NType &type, NTypeDecl &next)
		: line(line), index(index), id(id), type(type), next(next) {}
	NTypeDecl(int line, int index, Symbol &id, NType &type)
		: line(line), index(index), id(id), type(type) { next = NULL; }
	virtual llvm::Value *codeGen(CodeGenContext &context);
}

class NVarDecl : public NDecl
{
public:
	Symbol &id;
	NType &type;
	NExpr &initValue;
	// boolean escape = true;

	NVarDecl(int line, int index, Symbol &id, NExpr &initValue, NType &type)
		: line(line), index(index), id(id), initValue(initValue), type(type) {}
	NVarDecl(int line, int index, Symbol &id, NExpr &initValue)
		: line(line), index(index), id(id), initValue(initValue) { type = NULL; }

	virtual llvm::Value *codeGen(CodeGenContext &context);
}

/* Types */

class NArrayType : public NType
{
public:
	Symbol &id;

	NArrayType(int line, int index, Symbol &id)
		: line(line), index(index), id(id) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);

}

class NRecordType : public NType
{
public:
	NFieldTypeList &fields;

	NRecordType(int line, int index, NFieldTypeList &fields)
		: line(line), index(index), fields(fields) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
}

class NNameType : public NType
{
public:
	Symbol &id;

	NNameType(int line, int index, Symbol &id)
		: line(line), index(index), id(id) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
}

/* Variables */

class NSimpleVar : public NVar
{
public:
	Symbol &id;

	NSimpleVar(int line, int index, Symbol &id)
		: line(line), index(index), id(id) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
}

class NFieldVar : public NVar
{
public:
	NVar &var;
	Symbol &id;

	NFieldVar(int line, int index, NVar &var, Symbol &id)
		: line(line), index(index), var(var), id(id) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
}

class NSubscriptVar : public NVar
{
public:
	NVar &var;
	NExpr &index;

	NSubscriptVar(int line, int index, NVar &var, NExpr &index)
		: line(line), index(index), var(var), index(index) {}
	virtual llvm::Value *codeGen(CodeGenContext &context);
}

// class NIdentifier : public NExpr
// {
// public:
// 	std::string id;
// 	NIdentifier(const std::string &id) : id(id) {}
// 	virtual llvm::Value *codeGen(CodeGenContext &context);
// };

// class NMethodCall : public NExpr
// {
// public:
// 	const NIdentifier &id;
// 	ExpressionList arguments;
// 	NMethodCall(const NIdentifier &id, ExpressionList &arguments) : id(id), arguments(arguments) {}
// 	NMethodCall(const NIdentifier &id) : id(id) {}
// 	virtual llvm::Value *codeGen(CodeGenContext &context);
// };

// class NBinaryOperator : public NExpr
// {
// public:
// 	int op;
// 	NExpr &lhs;
// 	NExpr &rhs;
// 	NBinaryOperator(NExpr &lhs, int op, NExpr &rhs) : lhs(lhs), rhs(rhs), op(op) {}
// 	virtual llvm::Value *codeGen(CodeGenContext &context);
// };

// class NAssignment : public NExpr
// {
// public:
// 	NIdentifier &lhs;
// 	NExpr &rhs;
// 	NAssignment(NIdentifier &lhs, NExpr &rhs) : lhs(lhs), rhs(rhs) {}
// 	virtual llvm::Value *codeGen(CodeGenContext &context);
// };

// class NBlock : public NExpr
// {
// public:
// 	StatementList statements;
// 	NBlock() {}
// 	virtual llvm::Value *codeGen(CodeGenContext &context);
// };

// class NExprDecl : public NDecl
// {
// public:
// 	NExpr &expression;
// 	NExprDecl(NExpr &expression) : expression(expression) {}
// 	virtual llvm::Value *codeGen(CodeGenContext &context);
// };

// class NReturNDecl : public NDecl
// {
// public:
// 	NExpr &expression;
// 	NReturNDecl(NExpr &expression) : expression(expression) {}
// 	virtual llvm::Value *codeGen(CodeGenContext &context);
// };

// class NVarDecl : public NDecl
// {
// public:
// 	const NIdentifier &type;
// 	NIdentifier &id;
// 	NExpr *assignmentExpr;
// 	NVarDecl(const NIdentifier &type, NIdentifier &id) : type(type), id(id) { assignmentExpr = NULL; }
// 	NVarDecl(const NIdentifier &type, NIdentifier &id, NExpr *assignmentExpr) : type(type), id(id), assignmentExpr(assignmentExpr) {}
// 	virtual llvm::Value *codeGen(CodeGenContext &context);
// };

// class NExternDeclaration : public NDecl
// {
// public:
// 	const NIdentifier &type;
// 	const NIdentifier &id;
// 	VariableList arguments;
// 	NExternDeclaration(const NIdentifier &type, const NIdentifier &id,
// 					   const VariableList &arguments) : type(type), id(id), arguments(arguments) {}
// 	virtual llvm::Value *codeGen(CodeGenContext &context);
// };

// class NFunctionDeclaration : public NDecl
// {
// public:
// 	const NIdentifier &type;
// 	const NIdentifier &id;
// 	VariableList arguments;
// 	NBlock &block;
// 	NFunctionDeclaration(const NIdentifier &type, const NIdentifier &id,
// 						 const VariableList &arguments, NBlock &block) : type(type), id(id), arguments(arguments), block(block) {}
// 	virtual llvm::Value *codeGen(CodeGenContext &context);
// };
