import struct
import binascii
import zlib

width, height = 100, 100

# IHDR
ihdr_data = struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0)
ihdr_chunk = struct.pack(">I", 13) + b"IHDR" + ihdr_data
ihdr_crc = struct.pack(">I", binascii.crc32(b"IHDR" + ihdr_data) & 0xFFFFFFFF)
ihdr_full = ihdr_chunk + ihdr_crc

# Image data (solid blue RGB #0000FF, filter 0)
row = b'\x00' + b'\x00\x00\xff' * width
scanlines = row * height

# Compress (level=1 often fixed Huffman)
compressed = zlib.compress(scanlines, level=1)

# IDAT
idat_chunk = struct.pack(">I", len(compressed)) + b"IDAT" + compressed
idat_crc = struct.pack(">I", binascii.crc32(b"IDAT" + compressed) & 0xFFFFFFFF)
idat_full = idat_chunk + idat_crc

# IEND
iend_full = b"\x00\x00\x00\x00IEND\xae\x42\x60\x82"

# Full PNG
png = b"\x89PNG\r\n\x1a\n" + ihdr_full + idat_full + iend_full

with open("fixed_huffman.png", "wb") as f:
    f.write(png)