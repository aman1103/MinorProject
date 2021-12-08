gcc ./Algorithms/Huffman/gthc.c ./Algorithms/Huffman/gtbitio.c ./Algorithms/Huffman/huf.c -o ./Execs/huffman
echo "Compiled Huffman."
gcc ./Algorithms/RLE/rle.c -o ./Execs/rle
echo "Compiled RLE."
gcc ./Algorithms/RLE/efficient-rle.c -o ./Execs/efficientrle
echo "Compiled Efficient RLE."
gcc ./Algorithms/FGK/fgkc.c ./Algorithms/FGK/gtbitio.c ./Algorithms/FGK/huf.c -o ./Execs/adaptive-huffman
echo "Compiled FGK."
gcc ./Algorithms/LempelZiv/LZ77/LZ772.C -o ./Execs/lz77
echo "Compiled LZ77."
gcc ./Algorithms/LempelZiv/LZ78/LZWGT.c -o ./Execs/lz78
echo "Compiled LZ78."