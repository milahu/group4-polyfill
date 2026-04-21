#include <cstdint>
#include <vector>
#include <stdexcept>
#include <cstring>

#pragma pack(push, 1)

struct BMPFileHeader {
    uint16_t bfType = 0x4D42; // 'BM'
    uint32_t bfSize = 0;
    uint16_t bfReserved1 = 0;
    uint16_t bfReserved2 = 0;
    // uint32_t bfOffBits = 54 + 8; // header + palette
    uint32_t bfOffBits = 54 + (256 * 4); // header + palette
};

struct BMPInfoHeader {
    uint32_t biSize = 40;
    int32_t  biWidth = 0;
    int32_t  biHeight = 0;
    uint16_t biPlanes = 1;
    uint16_t biBitCount = 8; // we use 8-bit grayscale output (simplest for web)
    uint32_t biCompression = 0;
    uint32_t biSizeImage = 0;
    int32_t  biXPelsPerMeter = 2835;
    int32_t  biYPelsPerMeter = 2835;
    uint32_t biClrUsed = 0;
    uint32_t biClrImportant = 0;
};

#pragma pack(pop)

static inline uint32_t row_stride(uint32_t width) {
    // BMP rows are padded to 4-byte boundary
    return (width + 3) & ~3;
}

std::vector<uint8_t> encode_bmp_grayscale(
    const std::vector<uint8_t>& pixels,   // 0..255 per pixel
    uint32_t width,
    uint32_t height)
{
    if (pixels.empty() || width == 0 || height == 0)
        throw std::runtime_error("Invalid input");

    uint32_t stride = row_stride(width);
    uint32_t image_size = stride * height;

    BMPFileHeader file_header;
    BMPInfoHeader info_header;

    info_header.biWidth = width;
    info_header.biHeight = height;

    info_header.biSizeImage = image_size;

    file_header.bfSize = sizeof(BMPFileHeader)
                        + sizeof(BMPInfoHeader)
                        + 256 * 4 // grayscale palette
                        + image_size;

    std::vector<uint8_t> out;
    out.resize(file_header.bfSize);

    uint8_t* p = out.data();

    // --- headers ---
    std::memcpy(p, &file_header, sizeof(file_header));
    p += sizeof(file_header);

    std::memcpy(p, &info_header, sizeof(info_header));
    p += sizeof(info_header);

    // --- grayscale palette (256 entries) ---
    for (int i = 0; i < 256; i++) {
        *p++ = (uint8_t)i; // B
        *p++ = (uint8_t)i; // G
        *p++ = (uint8_t)i; // R
        *p++ = 0;          // reserved
    }

    // --- pixel data (bottom-up BMP) ---
    const uint8_t* base = pixels.data();

    for (int y = height - 1; y >= 0; y--) {
        const uint8_t* row = base + y * width;

        for (uint32_t x = 0; x < width; x++) {
            *p++ = row[x];
        }

        for (uint32_t i = width; i < stride; i++) {
            *p++ = 0;
        }
    }

    return out;
}

std::vector<uint8_t> expand_1bit_to_8bit(
    const std::vector<uint8_t>& src,
    uint32_t width,
    uint32_t height)
{
    uint32_t pitch_in = (width + 7) / 8;

    std::vector<uint8_t> out(width * height);

    for (uint32_t y = 0; y < height; y++) {
        const uint8_t* row = src.data() + y * pitch_in;

        for (uint32_t x = 0; x < width; x++) {
            uint8_t byte = row[x / 8];
            uint8_t bit = (byte >> (7 - (x % 8))) & 1;

            out[y * width + x] = bit ? 0 : 255; // min-is-black
        }
    }

    return out;
}

std::vector<uint8_t> encode_bmp_1bit_fast(
    const std::vector<uint8_t>& pixels,
    uint32_t width,
    uint32_t height)
{
    const uint32_t row_in = (width + 7) / 8;
    const uint32_t row_out = ((width + 31) / 32) * 4;

    const uint32_t palette_size = 8; // 2 colors * 4 bytes
    const uint32_t header_size = 54 + palette_size;

    std::vector<uint8_t> out(header_size + row_out * height, 0);

    uint8_t* p = out.data();

    // BMPFileHeader

    // uint16_t bfType = 0x4D42; // 'BM'
    p[0] = 'B'; p[1] = 'M';

    // uint32_t bfSize = 0;
    uint32_t file_size = out.size();
    memcpy(p + 2, &file_size, 4);

    // uint16_t bfReserved1 = 0;

    // uint16_t bfReserved2 = 0;

    // uint32_t bfOffBits = 54 + (256 * 4); // header + palette
    uint32_t pixel_offset = header_size;
    memcpy(p + 10, &pixel_offset, 4);

    // BMPInfoHeader

    uint8_t* h = p + 14;

    //     uint32_t biSize = 40;
    uint32_t dib_size = 40;
    memcpy(h + 0, &dib_size, 4);

    //     int32_t  biWidth = 0;
    int32_t width_int32 = (int32_t)width;
    memcpy(h + 4, &width_int32, 4);

    //     int32_t  biHeight = 0;
    int32_t height_int32 = (int32_t)height;
    // int32_t height_int32 = -(int32_t)height; // negative height
    memcpy(h + 8, &height_int32, 4);

    //     uint16_t biPlanes = 1;
    uint16_t planes = 1;
    memcpy(h + 12, &planes, 2);

    //     uint16_t biBitCount = 8; // we use 8-bit grayscale output (simplest for web)
    uint16_t bpp = 1;
    memcpy(h + 14, &bpp, 2);

    //     uint32_t biCompression = 0;
    uint32_t compression = 0;
    memcpy(h + 16, &compression, 4);

    //     uint32_t biSizeImage = 0;
    uint32_t image_size = row_out * height;
    memcpy(h + 20, &image_size, 4);

    int32_t biXPelsPerMeter = 2835;
    memcpy(h + 24, &biXPelsPerMeter, 4);

    int32_t biYPelsPerMeter = 2835;
    memcpy(h + 28, &biXPelsPerMeter, 4);

    //     uint32_t biClrUsed = 0;

    //     uint32_t biClrImportant = 0;

    // --- PALETTE ---
    uint8_t* pal = p + 54;
    pal[0] = 255; pal[1] = 255; pal[2] = 255; pal[3] = 0; // white
    pal[4] = 0;   pal[5] = 0;   pal[6] = 0;   pal[7] = 0; // black

    // --- PIXELS ---
    uint8_t* dst = p + header_size;

    const uint8_t* base = pixels.data();

    for (uint32_t y = 0; y < height; y++) {
        const uint8_t* src = base + y * row_in;
        // note: here we vertically flip the image
        // by replacing y with (height - 1 - y)
        uint8_t* row = dst + (height - 1 - y) * row_out;

        memcpy(row, src, row_in);
        // padding already zeroed
    }

    return out;
}
