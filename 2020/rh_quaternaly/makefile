
main.pdf : main.md store-buffering.pdf
	pandoc $< -o $@ --pdf-engine=lualatex -V geometry:a4paper,margin=3.5cm

store-buffering.pdf : store-buffering.tex
	lualatex -shell-escape $<
