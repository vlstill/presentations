all: cover-letter.pdf pres.pdf pres-abstract.pdf

%.pdf : %.tex
	TEXINPUTS=.:../../styles/lecture: latexmk -pdf $<

.PHONY: all
