#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "tiff_binary/tiff.hpp"

static void print_image(const tiff_binary::TiffImage& img) {
    std::cout << "TIFF Parsed Successfully\n";
    std::cout << "------------------------\n";
    std::cout << "Width: " << img.width << "\n";
    std::cout << "Height: " << img.height << "\n";

    std::cout << "Compression: ";
    switch (img.compression) {
        case tiff_binary::NONE:
            std::cout << "NONE";
            break;
        case tiff_binary::CCITT_GROUP4:
            std::cout << "CCITT_GROUP4";
            break;
        case tiff_binary::JBIG2:
            std::cout << "JBIG2 (stub)";
            break;
        default:
            std::cout << "UNKNOWN";
            break;
    }
    std::cout << "\n";

    std::cout << "BitsPerSample: " << img.bits_per_sample << "\n";
    std::cout << "SamplesPerPixel: " << img.samples_per_pixel << "\n";

    std::cout << "StripOffset: " << img.strip_offset << "\n";
    std::cout << "StripByteCount: " << img.strip_byte_count << "\n";
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: tiff_cli <file.tiff>\n";
        return 1;
    }

    const std::string filename = argv[1];

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: cannot open file: " << filename << "\n";
        return 1;
    }

    // Load entire file into memory (fine for CLI testing)
    std::vector<uint8_t> data(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    try {
        auto img = tiff_binary::TiffParser::parse(data.data(), data.size());
        print_image(img);
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "TIFF parse error: " << e.what() << "\n";
        return 1;
    }
}
