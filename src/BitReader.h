#ifndef BITREADER
#define BITREADER
typedef unsigned int u32;
typedef unsigned char u8;
class BitReader{
    private:
    u32 bitOffset;
    public:
    u8* reader;
    BitReader(u8* _reader)
    :reader(_reader),bitOffset(0)
    {

    };

    u32 read_bit();
};
#endif