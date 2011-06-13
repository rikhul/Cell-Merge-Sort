echo ""
echo "1Mi, k8"
./testdata/merger_node 256 4096 1 r
./run_tests_k8.sh

echo ""
echo "2Mi, k8"
./testdata/merger_node 256 8192 1 r
./run_tests_k8.sh

echo ""
echo "4Mi, k8"
./testdata/merger_node 256 16384 1 r
./run_tests_k8.sh

echo ""
echo "8Mi, k8"
./testdata/merger_node 256 32768 1 r
./run_tests_k8.sh

echo ""
echo "16Mi, k8"
./testdata/merger_node 256 65536 1 r
./run_tests_k8.sh

echo ""
echo "32Mi, k8"
./testdata/merger_node 256 131072 1 r
./run_tests_k8.sh

echo ""
echo "64Mi, k8"
./testdata/merger_node 256 262144 1 r
./run_tests_k8.sh
