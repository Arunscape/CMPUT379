make clean
make

cat > test.txt << EOF
M disk0
C dir1 0
Y dir1
C file1 16
L
EOF
./fs test.txt


cat > test.txt << EOF
M disk0
EOF
./fs test.txt
