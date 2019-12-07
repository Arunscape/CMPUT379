bash test.sh
cp sample_test_4/fs code/cmake-build-debug
cp sample_test_4/clean_disk code/cmake-build-debug

cd sample_test_4
./fs trivial-input > stdout_result 2> stderr_result
diff --color stdout stdout_result
diff --color stderr stderr_result

xxd clean_disk > actual
xxd clean_disk_result > expected
diff --color expected actual
cd ..
