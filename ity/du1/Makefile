FILE=main

.PHONY: $(FILE).pdf all clean

all: $(FILE).pdf

$(FILE).pdf: $(FILE).tex
	latexmk -pdf -pdflatex="pdflatex -interaction=nonstopmode" -use-make $(FILE).tex
	
clean:
	latexmk -CA

