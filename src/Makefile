all: parser

clean:
	rm parser.cpp parser.hpp parser tokens.cpp

parser.cpp: myparser.y
	bison -d -o $@ $^
    
parser.hpp: parser.cpp

tokens.cpp: mylexer.l parser.hpp
	lex -o $@ $^

parser: parser.cpp main.cpp tokens.cpp node.cpp
	g++ -o $@ `llvm-config --libs core native --cxxflags --ldflags` $^