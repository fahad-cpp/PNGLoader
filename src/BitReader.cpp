#include "BitReader.h"
#include <iostream>

u32 BitReader::peekBits(u32 bitCount){
    u32 code = 0;
    for(int i=0;i<bitCount;i++){
        u32 globalPos = bitOffset + i;
        u32 byte = reader[globalPos / 8];
        u32 bit = (byte >> (globalPos % 8)) & 1;
        code = (code << 1) | bit;
    }
    return code;
}

u32 BitReader:: readBit(){
    u32 bit = (*reader >> bitOffset) & 1;
    bitOffset++;
    if(bitOffset == 8){
        bitOffset = 0;
        reader++;
        bytesPushed++;
    }
    return bit;
}

u32 BitReader::readBits(u8 bitCount){
    if(bitCount > 32){
        bitCount = 32;
    }
    u32 code = 0;
    for(u8 i=1;i<=bitCount;i++){
        code = (code << 1) | readBit();
    }

    return code;
}

u32 BitReader::readBitsREV(u8 bitCount){
    if(bitCount > 32){
        bitCount = 32;
    }
    u32 code = 0;
    for(u8 i=0;i<bitCount;i++){
        code = code | (readBit() << i);
    }

    return code;
}

void BitReader::skipCurByte(u32 amount){
    reader += amount;
    bytesPushed += amount;
    bitOffset = 0;
}

void BitReader::skipBit(){
    bitOffset++;
    if(bitOffset == 8){
        bitOffset = 0;
        bytesPushed++;
        reader++;
    }
}