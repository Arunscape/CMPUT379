rm -f clean_disk
./create_fs clean_disk
chmod 777 clean_disk
sed 60q trivial-input > test
./fs test
xxd clean_disk > dump

