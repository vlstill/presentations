ALL_MD=$(wildcard *.md)

CPROGS=$(wildcard [a-z]*.c)
SEQPROGS=$(patsubst %.c,_cs_%.c,$(CPROGS))
PWD=$(shell pwd)

all : mds $(SEQPROGS)

.PRECIOUS: %.pdf

mds :	$(ALL_MD:.md=.pdf)

%.tex : %.md paper.pdf
	pandoc --slide-level=2 -V documentclass=paradise-slide -s $< -t beamer -f markdown+raw_tex -o $@ --template=../../styles/paradiseBeamer/template.tex

%.pdf : %.tex %.bib
	rubber --pdf -I "../../styles/paradiseBeamer/" -o $@ $<

paper.pdf :
	curl -o $@ https://arxiv.org/pdf/1605.02185v1.pdf

_cs_%.c : %.c Makefile
	-cd ~/tmp/lazycseq && python2 cseq.py --rounds=10 --unwind=10 --softunwindbound -i $(PWD)/$<
