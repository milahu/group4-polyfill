#include <fstream>
#include <iostream>
#include "tiff_binary/tiff.hpp"

int main(int argc, char** argv) {
    if (argc < 2) return 1;

    std::ifstream f(argv[1], std::ios::binary);
    std::vector<uint8_t> data(
        (std::istreambuf_iterator<char>(f)),
        std::istreambuf_iterator<char>());

    // decode + print metadata or dump image
}
