#ifndef BITREADER
#define BITREADER
typedef unsigned int u32;
typedef unsigned char u8;
class BitReader{
    public:
    u8 bitOffset;
    u8 LEbitOffset;
    u32 bytesPushed;
    u8* reader;
    BitReader(u8* _reader)
    :reader(_reader),bitOffset(0),bytesPushed(0),LEbitOffset(0)
    {

    };

    u32 read_bit();
    u32 read_bitLE();
    u32 read_byte();
    u32 read_bits(u8 bitCount);
    u32 read_bitsLE(u8 bitCount);
    u32 read_bitsNUM(u8 bitCount);
    u32 read_short();
    void skip(u32 amount);
};
#endif