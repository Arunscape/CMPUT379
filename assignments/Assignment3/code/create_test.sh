#! /bin/bash
make clean
make
echo M disk0 > test.txt
echo C first 64 >> test.txt
./fs test.txt

echo M disk0 > test.txt
echo C sec 63 >> test.txt
./fs test.txt

echo M disk0 > test.txt
./fs test.txt
