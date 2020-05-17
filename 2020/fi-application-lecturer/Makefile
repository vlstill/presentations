all: cover-letter.pdf pres.pdf

%.pdf : %.tex
	TEXINPUTS=.:../../styles/lecture: latexmk -pdf $<

.PHONY: all
