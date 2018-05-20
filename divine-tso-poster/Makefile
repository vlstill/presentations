
TEXS=$(wildcard *.tex)
STYS=$(wildcard *.sty)

poster.pdf: $(TEXS) poster.bib $(STYS)
	latexmk -pdf -bibtex poster.tex
