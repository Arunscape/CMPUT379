make clean
make

cat > test.txt << EOF
M disk0
C file1 32
C file2 32
C file3 32
D file2
EOF
./fs test.txt


cat > test.txt << EOF
M disk0
EOF
./fs test.txt
