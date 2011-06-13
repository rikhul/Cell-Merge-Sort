echo "1Mi, 64 noder á 16384 random ints"
./testdata/merger_node 64 16384 1 r
./run_tests.sh

echo "2Mi, 64 noder á 32768 random ints"
./testdata/merger_node 64 32768 1 r
./run_tests.sh

echo "4Mi, 64 noder á 65536 random ints"
./testdata/merger_node 64 65536 1 r
./run_tests.sh

echo "8Mi, 64 noder á 131072 random ints"
./testdata/merger_node 64 131072 1 r
./run_tests.sh

echo "76,8M, 64 noder á 1,200,000 random ints"
./testdata/merger_node 64 1200000 1 r
./run_tests.sh
