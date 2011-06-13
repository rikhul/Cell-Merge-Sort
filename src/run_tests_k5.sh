echo ""
echo "opti-k5-eps_01_wc.txt"
echo ""

cp opti-k5-eps_1_wc.txt 05map.txt
./sorter_k5 sorted_blocks.bin
./sorter_k5 sorted_blocks.bin
./sorter_k5 sorted_blocks.bin

echo ""
echo "opti-k5-eps_5_wc.txt"
echo ""

cp opti-k5-eps_5_wc.txt 05map.txt
./sorter_k5 sorted_blocks.bin
./sorter_k5 sorted_blocks.bin
./sorter_k5 sorted_blocks.bin

echo ""
echo "opti-k5-eps_99_wc.txt"
echo ""

cp opti-k5-eps_99_wc.txt 05map.txt
./sorter_k5 sorted_blocks.bin
./sorter_k5 sorted_blocks.bin
./sorter_k5 sorted_blocks.bin
