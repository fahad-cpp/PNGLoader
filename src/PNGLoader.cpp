#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <math.h>
#include "HuffmanTree.h"
#include "BitReader.h"

typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned short u16;

struct Pixel{
    u8 R;
    u8 G;
    u8 B;
    u8 getChannel(u8 i){
        if(i == 0){
            return R;
        }else if(i == 1){
            return G;
        }else if(i == 2){
            return B;
        }else{
            return (u8)NULL;
        }
    }
    void setChannel(u8 i,u8 value){
        if(i == 0){
            R = value;
        }else if(i == 1){
            G = value;
        }else if(i == 2){
            B = value;
        }
    }
};

struct ParsedData{
    u32 width;
    u32 height;
    u8 bpp;
    u8 colorType;
    u8 compressionMethod;
    u8 filterMethod;
    u8 interlaceMethod;
    std::vector<char> compressedData;
    std::vector<u8> imageData;
};

char colorTypes[7][20] = {
    "Grayscale",            // 0
    "ERROR",
    "Truecolor",            // 2
    "Indexed",              // 3
    "Grayscale and Alpha",  // 4
    "ERROR",
    "Truecolor and Alpha"   // 6
};
char compressionNames[3][16] = {
    "No Compression",
    "Fixed Huffman",
    "Dynamic Huffman"
};

class Parser {
public:
    ~Parser() = default;

    bool parse(const std::string& filepath, ParsedData& parsedData) {
        bool result=true;
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file) {
            std::cerr << "Failed to open file " << filepath << "\n";
            return false;
        }
        //Get size
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        //Copy png file into buffer
        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            std::cerr << "Failed to read from file\n";
            return false;
        }

        //Check header
        const unsigned char pngHeader[8] = {
            0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A
        };
        if (std::memcmp(buffer.data(), pngHeader, 8) != 0) {
            std::cerr << "The provided file " << filepath << " is not a valid PNG file\n";
            return false;
        }

        const char* reader = buffer.data() + 8;
        const char* end = buffer.data() + size;

        //Read until the last byte
        while (reader + 8 <= end) {
            uint32_t length = readLittleEndian32(reader);
            reader += 4;

            std::string chunkType(reader, 4);
            reader += 4;

            if (chunkType == "IHDR" && reader + 13 <= end) {
                parsedData.width = readLittleEndian32(&reader[0]);
                parsedData.height = readLittleEndian32(&reader[4]);
                parsedData.bpp = static_cast<unsigned char>(reader[8]);
                parsedData.colorType = static_cast<unsigned char>(reader[9]);
                parsedData.compressionMethod = static_cast<unsigned char>(reader[10]);
                parsedData.filterMethod = static_cast<unsigned char>(reader[11]);
                parsedData.interlaceMethod = static_cast<unsigned char>(reader[12]);

                bool supported = (
                    (parsedData.bpp == 8) &&
                    (parsedData.colorType == 2) &&
                    (parsedData.filterMethod == 0) &&
                    (parsedData.interlaceMethod == 0) &&
                    (parsedData.compressionMethod == 0) 
                );
                if(!supported){
                    bool bppSupported = (parsedData.bpp == 8);
                    bool colorTypeSupported = (parsedData.colorType == 2);
                    bool interlaceMethodSupported = (parsedData.interlaceMethod == 0);
                    bool filterMethodSupported = (parsedData.filterMethod == 0);
                    bool compressionMethodSupported = (parsedData.compressionMethod == 0);

                    if(!bppSupported){
                        std::cout << "The png file has unsupported bits per channel:"<<(int)parsedData.bpp<<"\n";
                    }
                    if(!colorTypeSupported){
                        std::cout << "The png file has unsupported color type: "<<(int)parsedData.colorType << " (" << colorTypes[(int)parsedData.colorType] << ")\n";
                    }
                    if(!interlaceMethodSupported){
                        std::cout << "The png file has unsupported interlace method:"<<(int)parsedData.interlaceMethod<<"\n";
                    }
                    if(!filterMethodSupported){
                        std::cout << "The png file has unsupported filter method:"<<(int)parsedData.filterMethod<<"\n";
                    }
                    if(!compressionMethodSupported){
                        std::cout << "The png file has unsupported compression method:"<<(int)parsedData.compressionMethod<<"\n";
                    }
                    std::cerr<<"PNG file is not supported\n";
                    //return false;
                }
            }
            else if (chunkType == "IDAT") {
                parsedData.compressedData.insert(parsedData.compressedData.end(), reader, reader + length);
            }else if (chunkType == "IEND"){
                if(!decompressData(parsedData)){
                    result = false;
                }
            }else{
                std::cout << "Unhandled Chunktype "<<chunkType<<" Encountered\n";
            }

            reader += length + 4; // skip data and CRC
        }
        
        return result;
    }

    std::string byteAsBin(char value){
        std::string result = "";
        for (int i=7;i>-1;i--){
            int mask = pow(2,i);
            result += std::to_string(((value & mask) != 0));
        }
        return result;
    }

    std::string bitsAsBin(u32 bits){
        char bytes[4];
        bytes[0] = (static_cast<u8>(bits >> 24));
        bytes[1] = (static_cast<u8>(bits >> 16));
        bytes[2] = (static_cast<u8>(bits >> 8));
        bytes[3] = (static_cast<u8>(bits));

        std::string res[4];
        res[0] = byteAsBin(bytes[0]);
        res[1] = byteAsBin(bytes[1]);
        res[2] = byteAsBin(bytes[2]);
        res[3] = byteAsBin(bytes[3]);

        std::string result = res[0] + ' ' + res[1] + ' ' + res[2] + ' ' + res[3];

        return result;
    }

    bool decompressData(ParsedData& parsedData){
        //Block compression types
        enum BlockType{
            BTYPE_NO_COMPRESSION=0,
            BTYPE_FIXED_HUFFMAN,
            BTYPE_DYNAMIC_HUFFMAN,
            BTYPE_RESERVED
        };
        const char* reader = parsedData.compressedData.data();
        const char* end = parsedData.compressedData.data() + parsedData.compressedData.size();
        //skip zlib header CMF
        reader += 1;

        bool DICTexists = ((*reader & 32) != 0); //*reader & 00100000
        //Skip FLG
        reader += 1;
        if(DICTexists){
            //skip DICT ADLER32
            reader += 4;
        }
        std::cout << "Compressed data size: "<<parsedData.compressedData.size()<<" Bytes \n";
        
        BitReader bitReader((u8*)reader);
        while((bitReader.reader + 8) <= (u8*)end){
            //Block header
            bool lastblockbit = bitReader.readBit();
            u32 compressionType = bitReader.readBitsLE(2);
            u16 blockLength = 0;
            if(compressionType == BTYPE_NO_COMPRESSION){
                std::cout <<"Deflate block : BTYPE_NO_COMPRESSION\n";
                //skip header
                bitReader.skipCurByte(1);
                //Read LEN(2B)
                blockLength = bitReader.readBitsLE(16);
                std::cout<<"Block length : "<<blockLength<<"\n";
                //Read NLEN(2B)
                u16 nlen = bitReader.readBitsLE(16);
                //check validity
                if(u16((blockLength | nlen) + 1u) != 0){
                    std::cerr << "LEN NLEN do not match\n";
                    std::cerr << "PNG file is corrupt\n";
                    return false;
                }
                parsedData.imageData.insert(parsedData.imageData.end(),(u8*)bitReader.reader,(u8*)bitReader.reader+blockLength);
                //skip data
                bitReader.skipCurByte(blockLength);
            }else if(compressionType == BTYPE_FIXED_HUFFMAN){
                std::cout <<"Deflate block : BTYPE_FIXED_HUFFMAN\n";
                static HuffmanTree tree;
                bool endOfBlock = false;
                tree.initializeStaticDeflateTree();
                while(!endOfBlock){
                    u32 symbol = 0;
                    u32 code = 0;
                    for(int i=7;i<=9;i++){
                        code = bitReader.peekBits(i);
                        
                        //lookup code from tree
                        if((code >= tree.firstCode[i]) && (code < (tree.firstCode[i] + tree.ncodes[i]))){
                            u32 index = tree.firstSymbol[i] + (code - tree.firstCode[i]);
                            symbol = tree.symbols[index];
                            bitReader.skipBits(i);
                            break;
                        }

                        if(i==9){
                            std::cout << bitsAsBin(code);
                            std::cout << "No codes matched the tree (THIS SHOULD NOT HAPPEN)\n";
                            return false;
                        }
                    }
                    if(symbol < 256){
                        //std::cout << "code: " << bitsAsBin(code) << "\n";
                        std::cout << "literal: "<<symbol<<"\n";
                        parsedData.imageData.push_back((u8)symbol);
                    }
                    else if(symbol == 256){
                        endOfBlock = true;
                    }else{
                        u32 length=0;
                        if(symbol < 265){
                            length = symbol - 254;
                        }else if(symbol == 285){
                            length = 258;
                        }else if(symbol < 285){
                            BitRange range = tree.symbolRangeMap[symbol];
                            u32 extraBits = range.bitCount;
                            u32 offset = bitReader.readBitsLE(extraBits);
                            length = range.min + offset;
                        }else{
                            length = 0;
                            std::cerr << "Invalid bit\n";
                            return false;
                        }
                        
                        u32 distanceCode = bitReader.readBits(5);
                        if(distanceCode > 29){
                            //std::cerr << "Code: "<< code <<"\n";
                            std::cerr << "Invalid distance code: "<<distanceCode<<"\n";
                            return false;
                        }
                        BitRange range = tree.symbolRangeMap[distanceCode];
                        u32 extraBits = range.bitCount;
                        u32 offset = extraBits?bitReader.readBitsLE(extraBits):0;
                        u32 distance = range.min + offset;
                        if(length > distance){
                            std::cerr << "Invalid Length: "<<length<<" (Length cant be more than distance)\n";
                            return false;
                        }
                        std::cout << "pair: <" << length << ',' << distance << ">\n";
                        for(int i=0;i<length;i++){
                            parsedData.imageData.push_back((0u));
                        }
                    }

                }
                std::cout << "Byte Offset : "<< bitReader.bytesPushed << "\n";
                std::cout << "Bit Offset : "<< (int)bitReader.bitOffset << "\n";

            }else if(compressionType == BTYPE_DYNAMIC_HUFFMAN){
                std::cout <<"Unhandled Deflate block:BTYPE_DYNAMIC_HUFFMAN\n";
                u32 hLit = bitReader.readBitsLE(5) ;
                u32 hDist = bitReader.readBitsLE(5);
                u32 hClen = bitReader.readBitsLE(4);
                
                //Code Length's Lengths
                u32 cll[19] = {0};
                for(u32 i=0;i<=(hClen+4);i++){
                    cll[i] = bitReader.readBitsLE(3);
                }
                HuffmanTree clTree;
                u32 symbols[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
                clTree.setCodeLengths(symbols,cll,19);
                
                std::vector<u32> literalLengthCodeLength;
                literalLengthCodeLength.reserve(hLit+257);
                for(u32 i=0;i<(hLit+257);i++){
                    u32 bitCount = 0;
                    //get encoded literal/length's code lengths
                    u32 code = clTree.decode(bitReader,bitCount);
                    if(code == 0xffffffff)return false;
                    bitReader.skipBits(bitCount);

                    if(code <= 15){
                        literalLengthCodeLength.push_back(code);
                    }else if(code == 16){
                        u32 repeatLength = bitReader.readBitsLE(2);
                        u32 prevSymbol = literalLengthCodeLength.at(literalLengthCodeLength.size()-1);
                        for(u32 i=0;i<(repeatLength+3);i++){
                            literalLengthCodeLength.push_back(prevSymbol);
                        }
                    }else if(code == 17){
                        u32 repeatLength = bitReader.readBitsLE(3);
                        for(u32 i=0;i<(repeatLength+3);i++){
                            literalLengthCodeLength.push_back(0);
                        }
                    }else if(code == 18){
                        u32 repeatLength = bitReader.readBitsLE(7);
                        for(u32 i=0;i<(repeatLength+11);i++){
                            literalLengthCodeLength.push_back(0);
                        }
                    }
                }
                
                std::cout << "LiteralLengthCodeSize: " << literalLengthCodeLength.size()<<"\n";
                HuffmanTree literalLengthTree;
                std::vector<u32> literalSymbols;
                literalSymbols.reserve(literalLengthCodeLength.size());
                for(int i=0;i<literalLengthCodeLength.size();i++)literalSymbols.push_back(i);
                literalLengthTree.setCodeLengths(literalSymbols.data(),literalLengthCodeLength.data(),literalLengthCodeLength.size());
                // for(int i=0;i<literalLengthCodeLength.size();i++){
                //     std::cout << "literal i: "<< i << " value: "<< literalLengthCodeLength.at(i) << "\n";
                // }

                std::cout << "HLIT: "<<hLit<<"\n";
                std::cout << "HDIST: "<<hDist<<"\n";
                std::cout << "HCLEN: "<<hClen<<"\n";
                return false;
            }else{
                std::cout << "ERROR : INVALID COMPRESSION TYPE\n";
                return false;
            }
            if(lastblockbit)std::cout << "(Last Block)\n";
        }
        
        return true;
    }
    
    private:
    //Little endian
    static u32 readLittleEndian32(const char* data) {
        return (static_cast<unsigned char>(data[0]) << 24) |
        (static_cast<unsigned char>(data[1]) << 16) |
        (static_cast<unsigned char>(data[2]) << 8)  |
        (static_cast<unsigned char>(data[3]));
    }
    static u16 readLittleEndian16(const char* data){
        return (static_cast<u16>(data[0]) << 8)|
        (static_cast<u16>(data[1]));    
    }
    //Big endian
    static u32 readBigEndian32(const char* data) {
        return (static_cast<unsigned char>(data[0])) |
        (static_cast<unsigned char>(data[1]) << 8) |
        (static_cast<unsigned char>(data[2]) << 16)  |
        (static_cast<unsigned char>(data[3]) << 24);
    }
    static u16 readBigEndian16(const char* data) {
        return (static_cast<unsigned char>(data[0])) |
        (static_cast<unsigned char>(data[1]) << 8);
    }
};

u32 paethPredictor(u8 a,u8 b,u8 c){
    u32 pr = 0;
    int p = a + b - c;
    u32 pa = abs(p - a);
    u32 pb = abs(p - b);
    u32 pc = abs(p - c);
    if(pa <= pb && pa <= pc)pr = a;
    else if(pb <= pc)pr = b;
    else pr = c;

    return pr;
}

const u8* createDefilteredBuffer(const u8* buffer,u32 width,u32 height){
    const u8* reader = buffer;
    if(!(reader + (sizeof(u8)*width*height*3))){
        std::cerr << "Invalid buffer provided\n";
        return nullptr;
    }
    u8* returnBuffer = (u8*)malloc(sizeof(u8)*(width*height*3));
    u8* writer = returnBuffer;
    Pixel prevPixel = {0,0,0};
    std::vector<Pixel> prevScanline(width,{0,0,0});
    //Per scanline
    for(u32 y=0;y<height;y++){
        int filterType = u32(reader[0]);
        std::vector<Pixel> currentScanline(width,{0,0,0});
        //skip filter byte
        reader += 1;
        prevPixel = {0,0,0};
         /*
               c       b
            |R|G|B| |R|G|B|
               a       x
            |R|G|B| |R|G|B| -> Current Pixel

            filtering types

            defiltering:
            0 = none    : reconstructed x[channel] = filtered x[channel]
            1 = sub     : reconstructed x[channel] = filtered x[channel] + filtered a[channel]
            2 = up      : reconstructed x[channel] = filtered x[channel] + filtered b[channel]
            3 = average : reconstructed x[channel] = filtered x[channel] + floor((filtered a[channel] + filtered b[channel]) / 2)
            4 = paeth   : reconstructed x[channel] = filtered x[channel] + paeth(a,b,c)
        */
        //Per Pixel
        for(u32 x=0;x<width;x++){
            Pixel curPixel = {0,0,0};
            //Per channel
            for(u32 i=0;i<3;i++){
                u8 value= reader?reader[i]:0;
                u8 a = (x>0?prevPixel.getChannel(i):0);
                u8 b = (y>0?prevScanline[x].getChannel(i):0);
                u8 c = (x>0?prevScanline[x-1].getChannel(i):0);
                if(filterType == 1){
                    value = value + a;
                }else if(filterType == 2){
                    value = value + b;
                }else if(filterType == 3){
                    value = value + floor((a + b)/ 2.f);
                }else if(filterType == 4){
                    value = value + paethPredictor(a,b,c);
                }else{
                    value = value;
                }
                curPixel.setChannel(i,value);
                writer[i] = curPixel.getChannel(i);
            }
            prevPixel = curPixel;
            currentScanline[x] = curPixel;
            reader+=3;
            writer+=3;
        }
        prevScanline = currentScanline;
    }
    //std::cout << "Buffer filtered.\n";
    return returnBuffer;
}

void deleteBuffer(const u8* buffer){
    free((void*)buffer);
}

void defilterAndOutput(const u8* buffer,u32 width,u32 height){
    //Get Defiltered Buffer
    const u8* rgbBuffer = createDefilteredBuffer(buffer,width,height);

    //Output Image to ppm
    std::ofstream ofs;
    ofs.open("imageoutput.ppm");
    if(!ofs.is_open()){
        std::cout << "Failed to open imageoutput.ppm\n";
        return;
    }
    const u8* reader = rgbBuffer;
    ofs << "P3\n" << width << " " <<  height <<"\n255\n";   
    //Per Scanline
    for(u32 y=0;y<height;y++){
        //Per Pixel
        for(u32 x=0;x<width;x++){
            //Per channel
            for(u32 i=0;i<3;i++){
                ofs << u32(reader[i]) << (i==2?"":" ");
            }
            reader += 3;
            ofs << '\n';
        }
    }
    
    deleteBuffer(rgbBuffer);
    ofs.close();
}

int main(int argc,char* argv[]) {

    const std::string filepath = argc<2?"res/test.png":argv[1];
    Parser parser;
    ParsedData parsedData;
    if (parser.parse(filepath, parsedData)) {
        // std::cout<<"---PNG--info---\n";
        // std::cout << "IHDR:\n";
        // std::cout << "\tWidth: " << parsedData.width << "\n";
        // std::cout << "\tHeight: " << parsedData.height << "\n";
        // std::cout << "\tBits per channel: " << int(parsedData.bpp) << "\n";
        // std::cout << "\tColor type: " << int(parsedData.colorType) << " (" << colorTypes[parsedData.colorType] << ")\n";
        // std::cout << "\tCompression Method: " << int(parsedData.compressionMethod) << "\n";
        // std::cout << "\tFilter Method: " << int(parsedData.filterMethod) << "\n";
        // std::cout << "\tInterlace Method: " << int(parsedData.interlaceMethod) << (parsedData.interlaceMethod?(" (Adam7 Interlace)"):(" (No interlace)")) << "\n";
        // std::cout << "IDAT:\n";

        // std::cout << std::fixed << "image data size: " << parsedData.imageData.size()<< " Bytes\n";
    }else{
        std::cerr << "Failed to parse the PNG\n";
        return 1;
    }
    defilterAndOutput(parsedData.imageData.data(),parsedData.width,parsedData.height);
    std::cout<<"Successfully parsed the png\n";
    //temperory for quick access
    system("start imageoutput.ppm");
    return 0;
}
