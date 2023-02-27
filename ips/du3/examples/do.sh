#!/bin/bash

#Author: Ježek

LOGIN="xlogin00"

mkdir $LOGIN
cd $LOGIN
mkdir results
cd results
for i in {1..3}
do
   mkdir $i
done
cd ../..

for i in {1..3}
do
   echo "---------------$i. run---------------"

	# 1st 
	cd p1-matmul
	make
	mv *.log.pdf p1-result.pdf
	make --silent clean
	mv p1-result.pdf ../$LOGIN/results/$i

	# 2nd
	cd ../p2-bandwidth
	make && make run
	./plot-all-results.sh
	mv *.pdf ../$LOGIN/results/$i
	make --silent clean-all

done

# get info to doc
cd ..
./0-getconf.sh > $LOGIN/hw-info.txt

echo "------------------DONE------------------"
