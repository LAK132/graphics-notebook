main.pdf: main.tex Makefile
	pdflatex main

.PHONY: main.pdf

clean:
	rm main.aux main.fdb_latexmk main.fls main.log main.synctex.gz main.toc main.out main.pdf