#include <iostream>
#include "codegen.h"
#include "node.h"
#include "semant.h"

using namespace std;

extern int yyparse();
extern NExpr *root;

void createCoreFunctions(CodeGenContext &context);

int main(int argc, char **argv)
{
	yyparse();
	root->print(0);
	Semant *analyzer = new Semant();
	root->traverse(analyzer);
	// cout << root << endl;
	// see http://comments.gmane.org/gmane.comp.compilers.llvm.devel/33877
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();
	CodeGenContext context;
	context.generateCode(root);
	context.runCode();

	return 0;
}
