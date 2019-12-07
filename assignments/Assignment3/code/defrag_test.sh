make clean
make

cat > test.txt << EOF
M disk0
C file1 1
B this is file 1
W file1 0
C file2 1
B this is file 2
W file2 0
C file3 1
B this is file 3
W file3 0
D file2
O
EOF
./fs test.txt


cat > test.txt << EOF
M disk0
EOF
./fs test.txt
