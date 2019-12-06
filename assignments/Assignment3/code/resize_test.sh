make clean
make

cat > test.txt << EOF
M disk0
C file1 1
B file1_message
W file1 0
B different_message
C file2 3
E file1 10
EOF
./fs test.txt


cat > test.txt << EOF
M disk0
EOF
./fs test.txt

make clean
make

cat > test.txt << EOF
M disk0
C file1 10
B file1_message
W file1 0
B different_message
C file2 3
E file1 1
EOF
./fs test.txt


cat > test.txt << EOF
M disk0
EOF
./fs test.txt
