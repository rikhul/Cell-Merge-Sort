echo ""
echo "opti-k6-eps_1_wc.txt"
echo ""

cp opti-k6-eps_1_wc.txt 06map.txt
./sorter_k6 sorted_blocks.bin
./sorter_k6 sorted_blocks.bin
./sorter_k6 sorted_blocks.bin

echo ""
echo "opti-k6-eps_5_uw.txt"
echo ""

cp opti-k6-eps_5_uw.txt 06map.txt
./sorter_k6 sorted_blocks.bin
./sorter_k6 sorted_blocks.bin
./sorter_k6 sorted_blocks.bin

echo ""
echo "optx-k6-eps_5_wc.txt"
echo ""

cp optx-k6-eps_5_wc.txt 06map.txt
./sorter_k6 sorted_blocks.bin
./sorter_k6 sorted_blocks.bin
./sorter_k6 sorted_blocks.bin

echo ""
echo "opti-k6-eps_99_uw.txt"
echo ""

cp opti-k6-eps_99_uw.txt 06map.txt
./sorter_k6 sorted_blocks.bin
./sorter_k6 sorted_blocks.bin
./sorter_k6 sorted_blocks.bin
