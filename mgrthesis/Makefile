ALL_MD=$(wildcard *.md)

all : mds

.PRECIOUS: %.md.tex %.pdf

mds :	$(ALL_MD:.md=.pdf)

%.pdf : %.md
	env TEXINPUTS="../styles/paradiseBeamer/:.:" pandoc --slide-level=2 -V documentclass=paradise-slide -s $< -t beamer -f markdown+raw_tex -o $@ --template=../styles/paradiseBeamer/template.tex

watch :
	while true; do inotifywait -e close_write,moved_to,create .; sleep 1; make; done

.PHONY: watch
