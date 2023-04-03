FILE=proj3

.PHONY: $(FILE).pdf all clean

all: $(FILE).pdf

$(FILE).pdf: $(FILE).tex
	latexmk -pdf -pdflatex="pdflatex -interaction=nonstopmode" -use-make $(FILE).tex

over: all
	pdftk vzor.pdf multibackground $(FILE).pdf output overlay.pdf
	
clean:
	latexmk -CA

