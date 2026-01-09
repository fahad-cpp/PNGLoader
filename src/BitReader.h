#ifndef BITREADER
#define BITREADER
typedef unsigned int u32;
typedef unsigned char u8;
class BitReader{
    public:
    u8 bitOffset;
    u32 bytesPushed;
    u8* reader;
    BitReader(u8* _reader)
    :reader(_reader),bitOffset(0),bytesPushed(0)
    {

    };
    u32 readBit();
    u32 readBits(u8 bitCount);
    u32 readBitsLE(u8 bitCount);
    u32 peekBits(u32 bitCount) const;
    void skipBit();
    void skipBits(u32 bitCount);
    void skipCurByte(u32 amount);
};
#endif