all: myparser

OBJS = main.o myparser.o tokens.o print.o traverse.o semant.o type.o codegen.o corefn.o

LLVMCONFIG = llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cppflags` -std=c++11
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

clean:
	rm myparser.cpp myparser.hpp myparser tokens.cpp $(OBJS)

myparser.cpp: myparser.y
	bison -d -o $@ $^
    
myparser.hpp: myparser.cpp

tokens.cpp: mylexer.l myparser.hpp
	lex -o $@ $^

%.o: %.cpp
	g++ -g -c $(CPPFLAGS) -o $@ $<

codegen.o: codegen.cpp
	g++ -g -c $(CPPFLAGS) -o $@ $<

# myparser: main.cpp myparser.cpp tokens.cpp node.cpp semant.cpp type.cpp codegen.cpp
# 	g++ -g -o $@ `llvm-config --libs --cppflags --ldflags` $^

myparser: $(OBJS)
	g++ -g -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

test:
	./myparser < ../test/Good/queens.tig