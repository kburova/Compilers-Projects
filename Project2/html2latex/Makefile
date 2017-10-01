LEX=lex
YACC=yacc
CC=gcc

LFLAGS=
CFLAGS=-Wall -Wno-unused-label -Wno-unused -g

all: html2latex

html2latex: html2latex.lex.o
	$(CC) -o $@ $+

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.lex.c: %.l
	$(LEX) $(LFLAGS) -o $@ $<

%.tex: %.html html2latex
	./html2latex < $< > $@
	
%.pdf: %.tex
	pdflatex $<

test: html2latex test.pdf
test1: html2latex test1.pdf
test2: html2latex test2.pdf

clean:
	$(RM) *.o
	$(RM) *.lex.c
	$(RM) *.tex *.log *.aux *.pdf
	$(RM) html2latex

#.PHONY: clean all test
