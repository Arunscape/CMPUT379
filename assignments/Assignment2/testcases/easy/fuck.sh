
make clean-all
make clean-result
make
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./distwc 15 5  ../Book-1/06-AoS-Content-1.txt ../Book-1/06-AoS-Content-2.txt ../Book-1/06-AoS-Content-3.txt ../Book-1/06-AoS-Content-4.txt ../Book-1/06-AoS-Content-5.txt ../Book-1/06-AoS-Content-6.txt ../Book-1/06-AoS-Content-7.txt ../Book-1/06-AoS-Content-8.txt ../Book-1/06-AoS-Content-9.txt ../Book-1/06-AoS-Content-10.txt
