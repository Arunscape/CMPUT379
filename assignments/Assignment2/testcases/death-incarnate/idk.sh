#!/bin/bash

make clean-result

echo "Testing using everything"

FILES=""

for file in ../Book-2/part00{08..21}_split_001.txt
do
        FILES+=" $file"
done

for file in ../Book-2/part00{23..27}_split_001.txt
do
        FILES+=" $file"
done

for file in ../Book-2/part00{29..35}_split_001.txt
do
        FILES+=" $file"
done

for file in ../Book-2/part00{37..69}_split_001.txt
do
        FILES+=" $file"
done

for file in ../Book-1/06-AoS-Content-{1..92}.txt
do
        FILES+=" $file"
done

make

#echo "First run"
#./distwc 1 6 ${FILES}
#./validation 6 result-0.txt result-1.txt result-2.txt result-3.txt result-4.txt result-5.txt 1-correct_result-0.txt 1-correct_result-1.txt 1-correct_result-2.txt 1-correct_result-3.txt 1-correct_result-4.txt 1-correct_result-5.txt
#make clean-result

#echo "Second run"
#./distwc 20 6 ${FILES}
#./validation 6 result-0.txt result-1.txt result-2.txt result-3.txt result-4.txt result-5.txt 1-correct_result-0.txt 1-correct_result-1.txt 1-correct_result-2.txt 1-correct_result-3.txt 1-correct_result-4.txt 1-correct_result-5.txt
#make clean-result

echo "Third run"
./distwc 200 6 ${FILES}
./validation 6 result-0.txt result-1.txt result-2.txt result-3.txt result-4.txt result-5.txt 1-correct_result-0.txt 1-correct_result-1.txt 1-correct_result-2.txt 1-correct_result-3.txt 1-correct_result-4.txt 1-correct_result-5.txt
make clean-result

make clean-all
