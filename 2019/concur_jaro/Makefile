ALL_MD=$(wildcard *.md)
ALL_DOT=$(wildcard *.dot)

all : mds $(SEQPROGS)

.PRECIOUS: %.pdf

mds :	$(ALL_MD:.md=.pdf)

%.tex : %.md paper.pdf
	pandoc --slide-level=2 -V documentclass=paradise-slide -s $< -t beamer -f markdown+raw_tex -o $@ --template=../../styles/paradiseBeamer/template.tex

%.pdf : %.tex $(ALL_DOT:.dot=.pdf) defs.tex
	rubber --pdf -I "../../styles/paradiseBeamer/" -o $@ $<

%.pdf : %.dot
	dot -Tpdf $< -o $@

paper.pdf :
	wget https://link.springer.com/content/pdf/10.1007%2F978-3-319-52234-0_14.pdf -O paper.pdf
