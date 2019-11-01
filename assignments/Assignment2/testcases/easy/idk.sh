FILES=""

for file in ../Book-1/06-AoS-Content-{1..10}.txt
do
        FILES+=" $file"
done

make

./distwc 1 1 ${FILES}
./validation 1 result-0.txt 1-correct_result-0.txt
#./distwc 5 5 ${FILES}
