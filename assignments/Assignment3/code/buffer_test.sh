make clean
make

cat > test.txt << EOF
M disk0
C file1 32
C file2 32
C file3 32
C dir1 0
R file2 16
D dir1
D file2
D file1
D file3
B *&^##UGUR*(
EOF
./fs test.txt


cat > test.txt << EOF
M disk0
EOF
./fs test.txt
