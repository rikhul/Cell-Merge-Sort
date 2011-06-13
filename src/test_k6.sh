echo ""
echo "1Mi, k6"
./testdata/merger_node 64 16384 1 r
./run_tests_k6.sh

echo ""
echo "2Mi, k6"
./testdata/merger_node 64 32768 1 r
./run_tests_k6.sh

echo ""
echo "4Mi, k6"
./testdata/merger_node 64 65536 1 r
./run_tests_k6.sh

echo ""
echo "8Mi, k6"
./testdata/merger_node 64 131072 1 r
./run_tests_k6.sh

echo ""
echo "16Mi, k6"
./testdata/merger_node 64 262144 1 r
./run_tests_k6.sh

echo ""
echo "32Mi, k6"
./testdata/merger_node 64 524288 1 r
./run_tests_k6.sh

echo ""
echo "64Mi, k6"
./testdata/merger_node 64 1048576 1 r
./run_tests_k6.sh
