# LLVM Based Tiger Language Compiler
The Tiger language is a simple and compact imperative programming language, with integers (Integer) and string (String) variables, arrays (Array), records (Record), and nestable functions (Function). The language has no concept of statements, and the program consists of expressions.

Tiger is defined by Andrew Appel in the famous "Tiger Book"-"Modern Compiler Implementation in Java 1". For specific specifications, see "[Tiger Language Reference Manual](http://www.cs.columbia.edu/~sedwards/classes/2002/w4115/tiger.pdf)" by Prof. Stephen Edwards.

This course project aims to implement a simple Tiger language compiler. At the same time complete the visualization of the abstract syntax tree.

## Environment
- flex 2.6.4
- bison 3.0.4
- llvm 6.0.0
- g++ 7.5.0

## Build and test
To build the compiler:
```
cd src
make
```
To test the compiler:
```
./parser < [path_to_test_program]
```
e.g.
```
./parser < ../test/test1.tig
```

An alternative way to test the program on `queens.tig` (the program of the Eight Queen Problem):
```
make test
```


## Duty Assignment
- Lexer, Parser: Zhang Yisu
- AST, Semantic Analyzer: Wang Yanhao
- Code Generation: Pan Qifan