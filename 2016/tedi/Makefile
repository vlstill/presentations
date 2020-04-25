ALL_MD=$(wildcard *.md)

all : mds

.PRECIOUS: %.md.tex %.pdf %.tex

mds :	$(ALL_MD:.md=.pdf)

%.tex : %.md
	pandoc --slide-level=2 -V documentclass=paradise-slide -s $< -t beamer -f markdown+raw_tex -o $@ --template=../../styles/paradiseBeamer/template.tex
	sed -i $@ -r \
		-e 's/\\begSplit/\\begin{minipage}[t]{0.48\\textwidth}/' \
		-e 's/\\Split(\[(.*)\])?/\\end{minipage}\\hfill\2\\begin{minipage}[t]{0.48\\textwidth}/' \
		-e 's/\\endSplit/\\end{minipage}/'

%.pdf : %.tex defs.tex thesis.bib
	rubber --pdf -I../../styles/paradiseBeamer $<

.PRECIOUS: %.tex %.pdf
