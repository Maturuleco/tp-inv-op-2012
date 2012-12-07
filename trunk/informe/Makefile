PL = pdflatex
SOURCE = informe.tex
PDF = informe.pdf
TEX = *.tex

TRASH = *.log *.toc *.aux *.out

.PHONY: all clean *.tex

all: $(PDF)

$(PDF): $(TEX)
	$(PL) $(SOURCE)
	$(PL) $(SOURCE)
	$(PL) $(SOURCE)
	$(MAKE) clean

clean:
	rm -f $(TRASH)
