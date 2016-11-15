CC=gcc
CCFLAGS = -lfl
LEX = flex
LEXFLAGS =
YACC = bison
YACCFLAGS = -d -v -g

all: yacc lex compile

yacc: main.y
	$(YACC) -o y.tab.c $(YACCFLAGS) $^
lex: main.l
	$(LEX) -o lex.yy.c $(LEXFLAGS) $^
compile: lex.yy.c y.tab.c
	$(CC) $^ $(CCFLAGS) -o test

clean:
	rm -f *.c *.h test
