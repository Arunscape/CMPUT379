FILES=""

for file in ../Book-1/06-AoS-Content-{1..10}.txt
do
        FILES+=" $file"
done

make

./distwc 1 1 ${FILES}
