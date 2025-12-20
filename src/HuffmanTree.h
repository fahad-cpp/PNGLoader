#ifndef HUFFMANTREE
#define HUFFMANTREE
#include <vector>
typedef unsigned char u8;
typedef unsigned int u32;
struct HuffmanTree{
    u8 maxBit=0;
    std::vector<u32> codes;
    std::vector<u32> first_code;
    std::vector<u32> first_symbol;
    std::vector<u32> symbols;

    void initializeStaticDeflateTree();
    void setMaxBit(u8 maxCount);
};  
#endif