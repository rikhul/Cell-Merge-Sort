echo ""
echo "1Mi, k5"
./testdata/merger_node 32 32768 1 r
./run_tests_k5.sh

echo ""
echo "2Mi, k5"
./testdata/merger_node 32 65536 1 r
./run_tests_k5.sh

echo ""
echo "4Mi, k5"
./testdata/merger_node 32 131072 1 r
./run_tests_k5.sh

echo ""
echo "8Mi, k5"
./testdata/merger_node 32 262144 1 r
./run_tests_k5.sh

echo ""
echo "16Mi, k5"
./testdata/merger_node 32 524288 1 r
./run_tests_k5.sh

echo ""
echo "32Mi, k5"
./testdata/merger_node 32 1048576 1 r
./run_tests_k5.sh

echo ""
echo "64Mi, k5"
./testdata/merger_node 32 2097152 1 r
./run_tests_k5.sh
