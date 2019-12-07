make clean
make

cat > test.txt << EOF
M disk0
C file1 32
C file2 32
C file3 32
D file2
D file1
D file3
C dir1 0
D dir1
EOF
./fs test.txt


cat > test.txt << EOF
M disk0
EOF
./fs test.txt
