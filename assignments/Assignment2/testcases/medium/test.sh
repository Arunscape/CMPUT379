#!/bin/bash

echo "Testing using the medium test cases"

FILES=""

for file in ../Book-1/06-AoS-Content-{1..10}.txt
do
        FILES+=" $file"
done

for file in ../Book-2/part00{08..20}_split_001.txt
do
        FILES+=" $file"
done

make

echo "First run"
./distwc 1 1 ${FILES}
./validation 1 result-0.txt 1-correct_result-0.txt
make clean-result

echo "Second run"
./distwc 5 1 ${FILES}
./validation 1 result-0.txt 1-correct_result-0.txt
make clean-result

echo "Third run"
./distwc 5 6 ${FILES}
./validation 6 result-0.txt result-1.txt result-2.txt result-3.txt result-4.txt result-5.txt 2-correct_result-0.txt 2-correct_result-1.txt 2-correct_result-2.txt 2-correct_result-3.txt 2-correct_result-4.txt 2-correct_result-5.txt
make clean-all
