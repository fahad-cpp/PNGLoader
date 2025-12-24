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
    //symbol to distance ranges map
    symbolRangeMap[0] = {0,1};
    symbolRangeMap[1] = {0,2};
    symbolRangeMap[2] = {0,3};
    symbolRangeMap[3] = {0,4};
    symbolRangeMap[4] = {1,5};
    symbolRangeMap[5] = {1,7};
    symbolRangeMap[6] = {2,9};
    symbolRangeMap[7] = {2,13};
    symbolRangeMap[8] = {3,17};
    symbolRangeMap[9] = {3,25};
    symbolRangeMap[10] = {4,33};
    symbolRangeMap[11] = {4,49};
    symbolRangeMap[12] = {5,65};
    symbolRangeMap[13] = {5,97};
    symbolRangeMap[14] = {6,129};
    symbolRangeMap[15] = {6,193};
    symbolRangeMap[16] = {7,257};
    symbolRangeMap[17] = {7,385};
    symbolRangeMap[18] = {8,513};
    symbolRangeMap[19] = {8,769};
    symbolRangeMap[20] = {9,1025};
    symbolRangeMap[21] = {9,1537};
    symbolRangeMap[22] = {10,2049};
    symbolRangeMap[23] = {10,3073};
    symbolRangeMap[24] = {11,4097};
    symbolRangeMap[25] = {11,6145};
    symbolRangeMap[26] = {12,8193};
    symbolRangeMap[27] = {12,12289};
    symbolRangeMap[28] = {13,16385};
    symbolRangeMap[29] = {13,24577};

    //static huffman length codes
    symbolRangeMap[257] = {0,3};
    symbolRangeMap[258] = {0,4};
    symbolRangeMap[259] = {0,5};
    symbolRangeMap[260] = {0,6};
    symbolRangeMap[261] = {0,7};
    symbolRangeMap[262] = {0,8};
    symbolRangeMap[263] = {0,9};
    symbolRangeMap[264] = {0,10};
    symbolRangeMap[265] = {1,11};
    symbolRangeMap[266] = {1,13};
    symbolRangeMap[267] = {1,15};
    symbolRangeMap[268] = {1,17};
    symbolRangeMap[269] = {2,19};
    symbolRangeMap[270] = {2,23};
    symbolRangeMap[271] = {2,27};
    symbolRangeMap[272] = {2,31};
    symbolRangeMap[273] = {3,35};
    symbolRangeMap[274] = {3,43};
    symbolRangeMap[275] = {3,51};
    symbolRangeMap[276] = {3,59};
    symbolRangeMap[277] = {4,67};
    symbolRangeMap[278] = {4,83};
    symbolRangeMap[279] = {4,99};
    symbolRangeMap[280] = {4,115};
    symbolRangeMap[281] = {5,131};
    symbolRangeMap[282] = {5,163};
    symbolRangeMap[283] = {5,195};
    symbolRangeMap[284] = {5,227};
    symbolRangeMap[285] = {0,258};

}

void HuffmanTree::setMaxBit(u8 maxCount){
    maxBit = maxCount;
    codes.resize(maxCount+1,0);
    first_code.resize(maxCount+1,0);
    first_symbol.resize(maxCount+1,0);
}