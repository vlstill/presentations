ALL_MD=$(wildcard *.md)

PWD=$(shell pwd)

all : mds

.PRECIOUS: %.pdf %.tex

mds :	$(ALL_MD:.md=.pdf)

%.tex : %.md Makefile
	perl -- ../../tools/md2tex.pl -t beamer --slide-level=2 \
		-V documentclass=paradise-slide --standalone --template=../../styles/paradiseBeamer/template.tex < $< > $@


%.pdf : %.tex Makefile
	TEXINPUTS=".:../../styles/paradiseBeamer/:" lualatex $< </dev/null
