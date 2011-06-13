echo ""
echo "1Mi, k7"
./testdata/merger_node 128 8192 1 r
./run_tests_k7.sh

echo ""
echo "2Mi, k7"
./testdata/merger_node 128 16384 1 r
./run_tests_k7.sh

echo ""
echo "4Mi, k7"
./testdata/merger_node 128 32768 1 r
./run_tests_k7.sh

echo ""
echo "8Mi, k7"
./testdata/merger_node 128 65536 1 r
./run_tests_k7.sh

echo ""
echo "16Mi, k7"
./testdata/merger_node 128 131072 1 r
./run_tests_k7.sh

echo ""
echo "32Mi, k7"
./testdata/merger_node 128 262144 1 r
./run_tests_k7.sh

echo ""
echo "64Mi, k7"
./testdata/merger_node 128 524288 1 r
./run_tests_k7.sh
