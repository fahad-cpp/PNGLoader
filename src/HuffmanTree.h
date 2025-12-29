#ifndef HUFFMANTREE
#define HUFFMANTREE

#include <vector>
#include <unordered_map>

typedef unsigned char u8;
typedef unsigned int u32;

struct BitRange{
    u32 bitCount;
    u32 min;
};

struct HuffmanTree{
    u8 maxBit=0;
    std::vector<u32> codes;
    std::vector<u32> first_code;
    std::vector<u32> first_symbol;
    std::vector<u32> symbols;
    std::unordered_map<u32,BitRange> symbolRangeMap;

    void setCodeLengths(u32 cLen[],u32 cLenSize);
    void initializeStaticDeflateTree();
    void setMaxBit(u8 maxCount);
    bool getKMI(u32 cLen[],u32 cLenSize);
};

#endif