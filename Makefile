NAME = minscheme

CC = gcc
CFLAGS = -Wall -Og -g -Wno-write-strings

OBJ = test-utils syscalls simp-tree errors symtab interpreter $(NAME).yy $(NAME).tab
OBJS  = $(addsuffix .o,$(OBJ))

compile:
	make $(OBJS)

ft: # full program (lex + parse)
	make compile
	g++ -std=c++11 -o $(NAME) $(NAME).cpp $(OBJS)
	./$(NAME)

st: # symtab test
	make compile
	g++ -std=c++11 -o symtab-test symtab-test.cpp symtab.o
	./symtab-test

stt: # simp-tree test
	make compile
	make simp-tree-test.o
	g++ -std=c++11 -o simp-tree-test simp-tree-test.o $(OBJS)
	./simp-tree-test

it: # interpreter test
	make compile
	make interpreter-test.o
	g++ -std=c++11 -o interpreter-test interpreter-test.o $(OBJS)
	./interpreter-test

t: # lexer test
	make compile
	g++ -std=c++11 -o lexer-test lexer-test.cpp $(OBJS)
	./lexer-test

ta: # test all
	make st
	make lt
	make it

######################################################################## 
interpreter-test.o: interpreter-test.cpp
	g++ -std=c++11 $(CFLAGS) -c interpreter-test.cpp
simp-tree-test.o: simp-tree-test.cpp
	g++ -std=c++11 $(CFLAGS) -c simp-tree-test.cpp
######################################################################## 
test-utils.o: test-utils.c test-utils.h 
	$(CC) $(CFLAGS) -c test-utils.c

syscalls.o: syscalls.c syscalls.h
	$(CC) $(CFLAGS) -c syscalls.c

simp-tree.o: simp-tree.c simp-tree.h
	$(CC) $(CFLAGS) -c simp-tree.c

errors.o: errors.c errors.h
	$(CC) $(CFLAGS) -c errors.c

symtab.o: symtab.cpp symtab.h
	g++ $(CFLAGS) -c -std=c++11 symtab.cpp

interpreter.o: interpreter.cpp interpreter.h
	g++ $(CFLAGS) -c -std=c++11 interpreter.cpp

$(NAME).yy.o: $(NAME).l
	flex -o $(NAME).yy.c $(NAME).l
	$(CC) $(CFLAGS) -c $(NAME).yy.c

$(NAME).tab.o: $(NAME).y
	bison -dtv $(NAME).y
	$(CC) $(CFLAGS) -c $(NAME).tab.c  	
######################################################################## 

t1: $(NAME).l $(NAME).y
	flex -o $(NAME).yy.c $(NAME).l
	bison -dtv $(NAME).y
	$(CC) -c $(NAME).yy.c   # -> name.yy.o
	#$(CC) -Wall -c $(NAME).tab.c $(INCSRC) # -> name.tab.o
	#$(CC) -o $(NAME) $(NAME).tab.o $(NAME).yy.o simp-tree.o

clean:
	rm cal cal.output cal.tab.c cal.tab.h cal.tab.o cal.yy.c cal.yy.o\
	 simp-tree.h.gch 

ctags:
	rm tags


	#flex -o $(NAME).yy.c $(NAME).l
	#bison -dtv $(NAME).y
	#$(CC) -c test-utils.c
	#$(CC) -c syscalls.c
	#$(CC) -c simp-tree.c			#-> simp-tree.o
	#$(CC) -c errors.c				#-> errors.o
	#g++ -c -std=c++11 symtab.cpp	#-> symtab.o
	#$(CC) -c $(NAME).yy.c        			# -> name.yy.o
	#$(CC) -Wall -c $(NAME).tab.c  	# -> name.tab.o
	#g++ -std=c++11 -Og -o interpreter-test interpreter-test.cpp\
		#test-utils.o $(NAME).yy.o $(NAME).tab.o errors.o symtab.o\
		#simp-tree.o syscalls.o