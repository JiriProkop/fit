#!/bin/bash

#Author: Ježek

LOGIN="xproko47"

mkdir $LOGIN
cd $LOGIN
mkdir results
cd ..

# 1st 
cd p1-matmul
make
mv *.log.pdf p1-result.pdf
make clean --silent
mv p1-result.pdf ../$LOGIN/results

# 2nd
cd ../p2-bandwidth
make && make run
./plot-all-results.sh
mv *.pdf ../$LOGIN/results
make clean-all --silent

# get info to doc
cd ..
./0-getconf.sh > $LOGIN/hw-info.txt

echo "------------------DONE------------------"