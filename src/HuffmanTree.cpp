#include "HuffmanTree.h"
#include <iostream>
void HuffmanTree::initializeStaticDeflateTree(){
    /*
        Lit Value   Bits    Codes
        ---------   ---     -----
        256- 279    7       0000000 through
                            0010111
        
        0 - 143     8       00110000 through
                            10111111

        280 - 287   8       11000000 through
                            11000111

        144 - 255   9       110010000 through
                            111111111
    */
    symbols.resize(288);
    setMaxBit(9);

    codes[7] = 24;
    codes[8] = 144 + 8;
    codes[9] = 112;
    u32 code = 0;
    for(u8 i=1;i<=maxBit;i++){
        code = (code + codes[i-1]) << 1;
        first_code[i] = code;
    }
    //7-bit symbols
    first_symbol[7] = 0;
    for(size_t i=0;i<24;i++){
        symbols[i] = 256 + i;
    }

    //8-bit symbols
    first_symbol[8] = 24;
    for(size_t i=24;i<(24 + 144);i++){
        int dist = i - 24;
        symbols[i] = 0 + dist;
    }

    for(size_t i = (24 + 144); i < (24 + 144 + 8);i++){
        int dist = i - (24 + 144);
        symbols[i] = 280 + dist;
    }

    //9-bit symbols
    first_symbol[9] = (24 + 144 + 8);
    for(size_t i = (24 + 144 + 8); i < (24 + 144 + 8 + 112); i++){
        int dist = i - (24 + 144 + 8);
        symbols[i] = 144 + dist;
    }

    for(int i=0;i<symbols.size();i++){
        std::cout << "symbols["<<i<<"]: "<< symbols[i] << "\n";
    }

    for(int i=0;i<first_symbol.size();i++){
        std::cout << "first_symbol["<<i<<"]: " << first_symbol[i] << "\n";
    }
}

void HuffmanTree::setMaxBit(u8 maxCount){
    maxBit = maxCount;
    codes.resize(maxCount+1,0);
    first_code.resize(maxCount+1,0);
    first_symbol.resize(maxCount+1,0);
}