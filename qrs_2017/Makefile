ALL_MD=$(wildcard *.md)

PWD=$(shell pwd)

all : mds

.PRECIOUS: %.pdf %.tex

mds :	$(ALL_MD:.md=.pdf)

%.tex : %.md Makefile
	pandoc --slide-level=2 -V documentclass=paradise-slide -s $< -t beamer -f markdown+raw_tex -o $@ --template=../styles/paradiseBeamer/template.tex
	sed -i -e 's/\\begSaveBox{\([^}]*\)}/\\global\\newsavebox\\\1\n\\global\\setbox\\\1=\\vbox\\bgroup/' \
		   -e 's/\\endSaveBox/\\egroup/' \
		$@

%.pdf : %.tex Makefile
	rubber --pdf -I "../styles/paradiseBeamer/" -o $@ $<

