#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace tiff_binary {

enum class Compression {
    None = 1,
    CCITT_G4 = 4,
    JBIG2 = 34712
};

struct Image {
    uint32_t width = 0;
    uint32_t height = 0;
    std::vector<uint8_t> pixels;
};

struct Decoder {
    virtual ~Decoder() = default;
    virtual bool decode(const uint8_t*, size_t,
                        uint32_t, uint32_t,
                        std::vector<uint8_t>&) = 0;
};

class TIFF {
public:
    static Image decode(const uint8_t* data, size_t size,
                        Decoder* ccitt,
                        Decoder* jbig2);
};

} // namespace tiff_binary
