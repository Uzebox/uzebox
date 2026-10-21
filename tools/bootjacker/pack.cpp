/* SPDX-License-Identifier: GPL-3.0-or-later */
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using Bytes = std::vector<uint8_t>;

static void require(bool ok, const char *message) {
    if (!ok) throw std::runtime_error(message);
}

struct Image {
    Bytes data = Bytes(65536, 0xff);
    unsigned first = 65536, last = 0;
};

static unsigned digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    throw std::runtime_error("Invalid HEX digit");
}

static Image read_hex(const char *path) {
    std::ifstream input(path);
    require(bool(input), "Cannot open HEX file");
    Image image;
    std::vector<bool> used(65536);
    uint32_t base = 0;
    bool ended = false;
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        require(!ended && line[0] == ':' && line.size() >= 11 && line.size() % 2,
                "Malformed HEX record or data after EOF");
        Bytes r;
        unsigned sum = 0;
        for (size_t i = 1; i < line.size(); i += 2) {
            r.push_back((digit(line[i]) << 4) | digit(line[i + 1]));
            sum += r.back();
        }
        require(r.size() == size_t(r[0]) + 5 && !(sum & 255), "HEX length/checksum error");
        unsigned n = r[0], address = (r[1] << 8) | r[2], type = r[3];
        if (type == 0) {
            uint64_t start = uint64_t(base) + address;
            require(start + n <= 65536, "HEX address outside ATmega644 flash");
            for (unsigned i = 0; i < n; ++i) {
                unsigned a = unsigned(start) + i;
                require(!used[a], "Overlapping HEX records");
                used[a] = true;
                image.data[a] = r[4 + i];
                image.first = std::min(image.first, a);
                image.last = std::max(image.last, a);
            }
        } else if (type == 1 && n == 0 && address == 0) ended = true;
        else if ((type == 2 || type == 4) && n == 2 && address == 0)
            base = uint32_t((r[4] << 8) | r[5]) << (type == 2 ? 4 : 16);
        else require((type == 3 || type == 5) && n == 4, "Unsupported HEX record");
    }
    require(!input.bad() && ended && image.first < 65536, "Missing HEX EOF/data");
    return image;
}

static uint32_t crc32(const Bytes &data) {
    uint32_t crc = 0xffffffffu;
    for (uint8_t b : data) {
        crc ^= b;
        for (unsigned i = 0; i < 8; ++i) crc = (crc >> 1) ^ ((crc & 1) ? 0xedb88320u : 0);
    }
    return ~crc;
}

static void write(const char *path, const std::string &data) {
    std::ifstream old(path, std::ios::binary);
    std::string existing((std::istreambuf_iterator<char>(old)), std::istreambuf_iterator<char>());
    if (old && existing == data) return;
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(data.data(), data.size());
    out.close();
    require(bool(out), "Cannot write output");
}

static void payload(const char *path) {
    Image image = read_hex(path);
    require(image.first == 0xf000, "Payload must start at 0xF000 and stay within 0xFFFF");
    Bytes data(image.data.begin() + 0xf000, image.data.end());
    require(data[2] == 0xb0 && data[3] == 0x07 && data[4] == 0x10 && data[5] == 0xad,
            "Payload has no bootloader API signature");
    for (unsigned i = 6; i < 8; ++i)
        require((data[i] >> 4) <= 9 && (data[i] & 15) <= 9, "Payload version is not BCD");
    const Bytes helpers[] = {
        {0xf8,0x94,0x87,0xbf,0xe8,0x95,0x78,0x94,0x07,0xb6,0x00,0xfc,0xfd,0xcf,0x08,0x95},
        {0x2f,0xb7,0xf8,0x94,0x87,0xbf,0xe8,0x95,0x2f,0xbf,0x07,0xb6,0x00,0xfc,0xfd,0xcf,0x08,0x95},
        {0x9f,0xb7,0xf8,0x94,0x87,0xbf,0xe8,0x95,0x9f,0xbf,0x07,0xb6,0x00,0xfc,0xfd,0xcf,0x08,0x95}
    };
    unsigned count = 0, site = 0;
    for (const Bytes &helper : helpers)
        for (unsigned at = 0xf00; at + helper.size() <= data.size(); at += 2)
            if (std::equal(helper.begin(), helper.end(), data.begin() + at)) { ++count; site = at; }
    require(count == 1, "Payload must contain exactly one supported flash helper in its last page");
    std::ostringstream meta, source;
    meta << "#define BJ_TARGET_TEXT \"TARGET: V" << (data[6] >> 4) << '.' << (data[6] & 15)
         << '.' << ((data[7] >> 4) * 10 + (data[7] & 15)) << "\"\n"
         << "#define BJ_PAYLOAD_CRC 0x" << std::hex << crc32(data) << "UL\n"
         << "#define BJ_NATIVE_BYTE 0x" << (0xf000 + site) << "\n";
    source << "#include <avr/pgmspace.h>\nconst unsigned char bj_payload[4096] PROGMEM = {\n";
    for (size_t i = 0; i < data.size(); ++i)
        source << unsigned(data[i]) << (i + 1 == data.size() ? "\n" : (i % 32 == 31 ? ",\n" : ","));
    source << "};\n";
    write("payload.c", source.str());
    write("payload_meta.h", meta.str());
}

static void little32(Bytes &data, unsigned at, uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) data[at + i] = value >> (8 * i);
}

static void uze(const char *input, const char *output) {
    Image image = read_hex(input);
    require(image.first == 0 && image.last < 0xf000, "Application overlaps bootloader or does not start at zero");
    image.data.resize((image.last + 512) & ~511u);
    Bytes header(512, 0);
    const char magic[] = "UZEBOX\1";
    std::copy(magic, magic + 8, header.begin());
    little32(header, 8, image.data.size());
    header[12] = 2026 & 255; header[13] = 2026 >> 8;
    const std::string title = "Bootjacker WRITE V5";
    const std::string author = "Lee Weber(D3thAdd3r)";
    const std::string description = "Upgrade legacy bootloader to V5";
    std::copy(title.begin(), title.end(), header.begin() + 14);
    std::copy(author.begin(), author.end(), header.begin() + 46);
    std::copy(description.begin(), description.end(), header.begin() + 339);
    little32(header, 334, crc32(image.data));
    header.insert(header.end(), image.data.begin(), image.data.end());
    write(output, std::string(header.begin(), header.end()));
}

int main(int argc, char **argv) {
    try {
        if (argc == 3 && std::string(argv[1]) == "payload") payload(argv[2]);
        else if (argc == 4 && std::string(argv[1]) == "writer")
            uze(argv[2], argv[3]);
        else throw std::runtime_error("Usage: pack payload bootloader.hex | pack writer input.hex output.UZE");
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "pack: " << e.what() << '\n';
        return 1;
    }
}
