TITLE=main
BIBFILE=references
IMAGES=$(wildcard img/*)

all: $(TITLE).pdf

clean:
	rm -f *.aux *.dvi *.log *.synctex.gz *.out *.nav *.toc *.snm *.run.xml *-blx.bib *.bbl *.blg *.bcf

cleanbib:
	bibtool -r bibstyle.txt -r biblatex -i $(BIBFILE).bib >tmp.bib
	mv tmp.bib $(BIBFILE).bib

$(TITLE).pdf: $(TITLE).tex $(TITLE).bib $(IMAGES)
	latexmk --pdf $(TITLE)

$(TITLE)-pub.pdf: $(TITLE)-pub.tex $(TITLE).bib $(IMAGES)
	latexmk --pdf $(TITLE)-pub

$(TITLE)-pub.tex: $(TITLE).tex
	sed 's/\\documentclass\[sigconf\]{acmart}/\\documentclass[sigconf, authorversion, screen]{acmart}/' $< > $@

# Using git latexdiff
# https://gitlab.com/git-latexdiff/git-latexdiff
#diff:
#	git latexdiff --main $(TITLE).tex --prepare "make remake" --exclude-textcmd="section,subsection" 11854ab HEAD

.PHONY: all clean cleanbib remake diff
