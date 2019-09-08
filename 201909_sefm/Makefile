ALL_MD=$(wildcard *.md)
ALL_DOT=$(wildcard *.dot)

PWD=$(shell pwd)

all : mds

.PRECIOUS: %.pdf

mds :	$(ALL_MD:.md=.pdf)

%.tex : %.md Makefile
	perl -- ../tools/md2tex.pl -f markdown+smart -t beamer --slide-level=2 \
		-V documentclass=paradise-slide --standalone --template=../styles/paradiseBeamer/template.tex < $< > $@

%.pdf : %.tex Makefile $(ALL_DOT:.dot=.pdf)
	$(info $^)
	rubber --pdf -I "../styles/paradiseBeamer/" -o $@ $<

%.pdf : %.dot Makefile
	dot -Tpdf $< > $@

