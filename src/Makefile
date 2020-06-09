all: myparser

OBJS = main.o myparser.o tokens.o node.o semant.o type.o codegen.o

LLVMCONFIG = llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cppflags` -std=c++11
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

clean:
	rm myparser.cpp myparser.hpp myparser tokens.cpp

myparser.cpp: myparser.y
	bison -d -o $@ $^
    
myparser.hpp: myparser.cpp

tokens.cpp: mylexer.l myparser.hpp
	lex -o $@ $^

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

# myparser: main.cpp myparser.cpp tokens.cpp node.cpp semant.cpp type.cpp codegen.cpp
# 	g++ -g -o $@ `llvm-config --libs --cppflags --ldflags` $^

myparser: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)