ALL_DOT=$(wildcard *.dot)

PWD=$(shell pwd)

all : pres.pdf

.PRECIOUS: %.pdf

%.pdf : %.tex Makefile $(ALL_DOT:.dot=.pdf)
	$(info $^)
	TEXINPUTS=".:../../styles/paradiseBeamer/:" latexmk -pdflua -lualatex -shell-escape $< </dev/null

%.pdf : %.dot Makefile
	dot -Tpdf $< > $@

