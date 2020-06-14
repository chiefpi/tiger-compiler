#include <iostream>
#include <stdlib.h>
#include "codegen.h"
#include "node.h"
#include "semant.h"
#include "utils.h"

using namespace std;

extern int yyparse();
extern NExpr *root;

int main(int argc, char **argv)
{
	InputParser input(argc, argv);
	// const char *filename = input.getCmdOption("-f").c_str;
	// FILE *fp = fopen(filename, "a");
	// if (fp == NULL)
	// 	throw runtime_error(strerror(errno));

	yyparse();
	if (input.cmdOptionExists("-v"))
		root->print(0);
	else
	{
		Semant *analyzer = new Semant();
		root->traverse(analyzer);

		llvm::InitializeNativeTarget();
		llvm::InitializeNativeTargetAsmPrinter();
		llvm::InitializeNativeTargetAsmParser();
		CodeGenContext context;
		context.generateCode(root);

		if (!input.cmdOptionExists("-i"))
			context.runCode();
	}

	return 0;
}
