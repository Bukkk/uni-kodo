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

std::vector<uint8_t> tovec(std::string const& s)
{
    std::vector<uint8_t> res {};

    auto size = s.size();
    res.resize(size);

    for (uint64_t i {}; i < size; ++i) {
        res[i] = s[i];
    }

    return res;
}

int main()
{
    hamming::c_hamming_8_4 hamm {};

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
    }

    {
        std::vector<uint8_t> d { 0xf0 };
        std::cout << "bez zmian= " << (uint32_t)hamming::c_hamming_8_4::hamming_syndrome(hamm.encode(d)[0]) << "\n";
        std::cout << "  p bez zmian= " << (uint32_t)hamming::c_hamming_8_4::parity_syndrome(hamm.encode(d)[0]) << "\n";

        std::cout << "pierwszy bit 0= " << (uint32_t)hamming::c_hamming_8_4::hamming_syndrome(hamm.encode(d)[0] & 0xfe) << "\n";
        std::cout << "  pierwszy bit 0= " << (uint32_t)hamming::c_hamming_8_4::parity_syndrome(hamm.encode(d)[0] & 0xfe) << "\n";

        std::cout << "pierwszy bit 1= " << (uint32_t)hamming::c_hamming_8_4::hamming_syndrome(hamm.encode(d)[0] | 0x01) << "\n";
        std::cout << "  pierwszy bit 1= " << (uint32_t)hamming::c_hamming_8_4::parity_syndrome(hamm.encode(d)[0] | 0x01) << "\n";

        for (uint32_t i {}; i < 8; ++i) {
            std::cout << "b" << i << "= " << (uint32_t)hamming::c_hamming_8_4::hamming_syndrome(0x1 << i) << "\n";
            std::cout << "  p" << i << "= " << (uint32_t)hamming::c_hamming_8_4::parity_syndrome(0x1 << i) << "\n";
        }

        std::cout << "b01= " << (uint32_t)hamming::c_hamming_8_4::hamming_syndrome(0x03) << "\n";
        std::cout << "  p01= " << (uint32_t)hamming::c_hamming_8_4::parity_syndrome(0x03) << "\n";

        std::cout << "%%%%%%%%%%%%%%%%%%%%%";
    }

    std::vector<uint8_t> data { 'f', 'h', 'w', 0x00, 0xff };
    auto encoded = hamm.encode(data);
    auto decoded = hamm.decode(encoded);

    assert(decoded == data);

    auto tekscik = tovec(std::string { "Opracowanie redakcyjne i przypisy: Maria Bailey, Anna Dyja, Marta Niedziałkowska, Agnieszka Paul, Magdalena Paul, Aleksandra Sekuła, Weronika Trzeciak" });

    assert(tekscik == hamm.decode(hamm.encode(tekscik)));

    std::string k = "qwertyuiopasdfghjklzxcvbnm";
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    k += k;
    auto tekscik2 = tovec(k);

    auto t2enc = hamm.encode(tekscik2);
    auto t2dec = hamm.decode(t2enc);
    assert(tekscik2 == t2dec);
}