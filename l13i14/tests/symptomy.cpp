#include "../src/hamming.hpp"
#include "../src/utils/misc.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>

std::ostream& operator<<(std::ostream& o, std::vector<uint8_t> const& v)
{
    for (auto const& k : v) {
        o << uint32_t { k } << ' ';
    }

    return o;
}

int main()
{
    std::cout << "bez zmian= " << (uint32_t)hamming::c_hamming_8_4::hamming_syndrome(0x00) << "\n";
    std::cout << "  p bez zmian= " << (uint32_t)hamming::c_hamming_8_4::parity_syndrome(0x00) << "\n";

    for (uint32_t i {}; i < 8; ++i) {
        std::cout << "b" << i << "= " << (uint32_t)hamming::c_hamming_8_4::hamming_syndrome(0x1 << i) << "\n";
        std::cout << "  p" << i << "= " << (uint32_t)hamming::c_hamming_8_4::parity_syndrome(0x1 << i) << "\n";
    }

    std::cout << "b01= " << (uint32_t)hamming::c_hamming_8_4::hamming_syndrome(0x03) << "\n";
    std::cout << "  p01= " << (uint32_t)hamming::c_hamming_8_4::parity_syndrome(0x03) << "\n";

    std::cout << "%%%%%%%%%%%%%%%%%%%%%";

    hamming::c_hamming_8_4 hamm {};

    std::vector<uint8_t> data {'f', 'h', 'w', 0x00, 0xff};
    auto encoded = hamm.encode(data);
    auto decoded = hamm.decode(encoded);

    assert(decoded == data);
}