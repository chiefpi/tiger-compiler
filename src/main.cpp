#include <iostream>
//#include "codegen.h"
#include "node.h"

using namespace std;

extern int yyparse();
extern NExpr *root;

//void createCoreFunctions(CodeGenContext &context);

int main(int argc, char **argv)
{
	yyparse();
	root->print(0);
	// cout << programBlock << endl;
	// see http://comments.gmane.org/gmane.comp.compilers.llvm.devel/33877
	// InitializeNativeTarget();
	// InitializeNativeTargetAsmPrinter();
	// InitializeNativeTargetAsmParser();
	// CodeGenContext context;
	// createCoreFunctions(context);
	// context.generateCode(*programBlock);
	// context.runCode();

	return 0;
}
