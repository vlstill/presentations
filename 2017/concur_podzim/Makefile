ALL_MD=$(wildcard *.md)
ALL_DOT=$(wildcard *.dot)

PWD=$(shell pwd)

all : mds

.PRECIOUS: %.pdf %.tex

mds :	$(ALL_MD:.md=.pdf)

%.tex : %.md Makefile
	perl -- ../../tools/md2tex.pl -t beamer --slide-level=2 \
		-V documentclass=paradise-slide --standalone --template=../../styles/paradiseBeamer/template.tex < $< > $@
	sed -i $@ -e 's/\\pause/\\onslide<+(1)->/g'

%.pdf : %.tex Makefile $(ALL_DOT:.dot=.pdf) paper.pdf
	$(info $^)
	rubber --pdf -I "../../styles/paradiseBeamer/" -o $@ $<

%.pdf : %.dot Makefile
	dot -Tpdf $< > $@

paper.pdf :
	wget "http://www.fit.vutbr.cz/research/groups/verifit/tools/predator/FIT-TR-2012-04.pdf" -O $@

