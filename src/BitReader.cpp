#include "BitReader.h"
#include <iostream>
u32 BitReader::read_bit(){
    u32 bit = *reader & (1 << (7-bitOffset));
    bit = (bit >> (7-bitOffset));
    bitOffset++;
    if(bitOffset > 7){
        reader++;
        bytesPushed++;
        bitOffset = 0;
    }
    return bit;
}

u32 BitReader::read_bitLE(){
    u32 bit = *reader & (1 << (LEbitOffset));
    bit = (bit >> (LEbitOffset));
    LEbitOffset++;
    if(LEbitOffset > 7){
        reader++;
        bytesPushed++;
        LEbitOffset = 0;
    }
    return bit;
}

u32 BitReader::read_bits(u8 bitCount){
    if(bitCount > 32){
        std::cerr << "bits can only be read upto 32 at a time\n";
        return 0;
    }
    u32 code = 0;
    for(u8 i=1;i<=bitCount;i++){
        code = (code << 1) | read_bit();
    }

    return code;
}

u32 BitReader::read_bitsLE(u8 bitCount){
    if(bitCount > 32){
        std::cerr << "bits can only be read upto 32 at a time\n";
        return 0;
    }
    u32 code = 0;
    for(u8 i=1;i<=bitCount;i++){
        code = (code << 1) | read_bitLE();
    }

    return code;
}

u32 BitReader::read_bitsLENUM(u8 bitCount){
    if(bitCount > 32){
        std::cerr << "bits can only be read upto 32 at a time\n";
        return 0;
    }
    u32 code = 0;
    for(u8 i=1;i<=bitCount;i++){
        code = code | (read_bitLE() << i);
    }

    return code;
}

u32 BitReader::read_bitsNUM(u8 bitCount){
    if(bitCount > 32){
        std::cerr << "bits can only be read upto 32 at a time\n";
        return 0;
    }
    u32 code = 0;
    for(u8 i=0;i<bitCount;i++){
        code = code | (read_bit() << i);
    }

    return code;
}

u32 BitReader::read_byte(){
    u32 byte = reader[0];
    bitOffset = 0;
    LEbitOffset = 0;
    bytesPushed++;
    reader++;

    return byte;
}

u32 BitReader::read_short(){
    u32 ret = (static_cast<unsigned char>(reader[0])) |
        (static_cast<unsigned char>(reader[1]) << 8);

    reader += 2;
    bytesPushed += 2;
    bitOffset = 0;
    LEbitOffset = 0;
    return ret;
}

void BitReader::skip(u32 amount){
    reader += amount;
    bytesPushed += amount;
    bitOffset = 0;
    LEbitOffset = 0;
}

void BitReader::skipBitLE(u32 amount){
    LEbitOffset++;
}