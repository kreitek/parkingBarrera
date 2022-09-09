#!/bin/sh
for FILENAME in *dot; do
	dot $FILENAME -Tsvg > $FILENAME.svg
	dot $FILENAME -Tpdf > $FILENAME.pdf
done
