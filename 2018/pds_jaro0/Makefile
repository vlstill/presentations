ALL_MD=$(wildcard *.md)
ALL_DOT=$(wildcard *.dot)

all : mds $(SEQPROGS)

.PRECIOUS: %.pdf

mds :	$(ALL_MD:.md=.pdf)

%.tex : %.md
	pandoc --slide-level=2 -V documentclass=paradise-slide -s $< -t beamer -f markdown+raw_tex -o $@ --template=../../styles/paradiseBeamer/template.tex

%.pdf : %.tex $(ALL_DOT:.dot=.pdf)
	rubber --pdf -I "../../styles/paradiseBeamer/" -o $@ $<

%.pdf : %.dot
	dot -Tpdf $< -o $@

