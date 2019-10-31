#!/bin/bash

echo "Testing"

FILES=""

for file in ../Book-1/06-AoS-Content-{1..10}.txt
do
        FILES+=" $file"
done

make

./distwc 1 1 ${FILES}

./validation 1 result-0.txt 1-correct_result-0.txt

make clean-result

./distwc 5 5 ${FILES}

./validation 5 result-0.txt result-1.txt result-2.txt result-3.txt result-4.txt 2-correct_result-0.txt 2-correct_result-1.txt 2-correct_result-2.txt 2-correct_result-3.txt 2-correct_result-4.txt

make clean-result

./distwc 15 5 ${FILES}

./validation 5 result-0.txt result-1.txt result-2.txt result-3.txt result-4.txt 2-correct_result-0.txt 2-correct_result-1.txt 2-correct_result-2.txt 2-correct_result-3.txt 2-correct_result-4.txt

make clean-all