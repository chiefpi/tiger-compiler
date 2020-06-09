all: myparser

clean:
	rm myparser.cpp myparser.hpp myparser tokens.cpp

myparser.cpp: myparser.y
	bison -d -o $@ $^
    
myparser.hpp: myparser.cpp

tokens.cpp: mylexer.l myparser.hpp
	lex -o $@ $^

myparser: main.cpp myparser.cpp tokens.cpp node.cpp semant.cpp type.cpp codegen.cpp
	g++ -g -o $@ `llvm-config --libs core native --cxxflags --ldflags` $^