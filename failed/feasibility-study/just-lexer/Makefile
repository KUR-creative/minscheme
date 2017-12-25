NAME = minscheme

CC = gcc
CFLAGS = -Wall -O0 -g 

t:
	$(CC) -c test-utils.c
	flex -o $(NAME).yy.c $(NAME).l
	bison -dtv $(NAME).y
	$(CC) -c $(NAME).yy.c        # -> name.yy.o
	$(CC) -Wall -c $(NAME).tab.c # -> name.tab.o
	g++ -std=c++11 -o lexer-test lexer-test.cpp\
		test-utils.o $(NAME).yy.o $(NAME).tab.o 
	./lexer-test

conly: 
	g++ -std=c++11 -o lexer-test lexer-test.cpp\
		test-utils.o $(NAME).yy.o $(NAME).tab.o 
	./lexer-test

o:
	$(CC) -c test-utils.c
	flex -o $(NAME).yy.c $(NAME).l
	bison -dtv $(NAME).y
	$(CC) -c $(NAME).yy.c        # -> name.yy.o
	$(CC) -Wall -c $(NAME).tab.c # -> name.tab.o

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
