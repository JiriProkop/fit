LOGIN=xproko47

README.pdf: README.md
	pandoc -V geometry:margin=2cm --variable urlcolor=blue $< -o $@

zip:
	zip $(LOGIN).zip *.feature README.md