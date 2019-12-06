make clean
make

cat > test.txt << EOF
M disk0
C file1 16
C file2 16
C file3 16
D file2
O
EOF
./fs test.txt


cat > test.txt << EOF
M disk0
EOF
./fs test.txt
