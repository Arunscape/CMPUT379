rm -rf sample_test_*
7z x sample_tests.zip
cd code
make clean
make
cp fs ../sample_test_1/
cp fs ../sample_test_2/
cp fs ../sample_test_3/
cp fs ../sample_test_4/
