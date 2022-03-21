#pragma once

// Container for bits statistics.
#include <cstdint>
#include <math.h>
struct BitStat {
    unsigned long long readCounter = 0; // Number of input bits.
    unsigned long long writeCounter = 0; // Number of output bits.
};

#include <cmath>
#include <string>
#include <vector>

#include "adaptive_model.hpp"

// class Statistics {
// public:
//     Statistics(std::vector<BitStat> stats)
//         : m_stats(stats)
//     {
//     }

//     double entropy()
//     {
//         unsigned long long n = 0uLL; // total number of bits written
//         for (size_t i = 0; i < m_stats.size(); i++)
//             n += m_stats[i].readCounter;

//         double h = 0;
//         for (size_t i = 0; i < m_stats.size(); i++) {
//             double p = (double)m_stats[i].readCounter / n;
//             if (p > 0)
//                 h += m_stats[i].readCounter * -log2(p);
//         }
//         return h / n;
//     }

//     double averageCodingLength()
//     {
//         unsigned long long inputBits = 0;
//         unsigned long long outputBits = 0;

//         for (size_t i = 0; i < m_stats.size(); i++) {
//             inputBits += m_stats[i].readCounter;
//             outputBits += m_stats[i].writeCounter;
//         }

//         return 8 * ((double)outputBits / inputBits); // bits per symbol (byte)
//     }

//     double compressionRatio()
//     {
//         unsigned long long inputBits = 0;
//         unsigned long long outputBits = 0;

//         for (size_t i = 0; i < m_stats.size(); i++) {
//             inputBits += m_stats[i].readCounter;
//             outputBits += m_stats[i].writeCounter;
//         }

//         return (1.0 - (double)outputBits / inputBits);
//     }

// private:
//     std::vector<BitStat> m_stats;
// };

inline auto vector_cast(const std::vector<unsigned char>& char_vector) -> std::vector<bool>
{
    std::vector<bool> bool_vector {};
    bool_vector.reserve(8 * char_vector.size());

    for (std::size_t byte_id {}; byte_id < char_vector.size(); ++byte_id) {
        for (uint32_t bit_id = 8; bit_id > 0; --bit_id) {
            // TODO nie wiem gdzie jest koniec. dodaja sie false na koncu jak to wplywa na dekompresje

            bool val = char_vector[byte_id] & (1 << (bit_id - 1));
            bool_vector.push_back(val);
        }
    }

    return bool_vector;
}

inline auto vector_cast(const std::vector<bool>& bool_vector) -> std::vector<unsigned char>
{
    std::vector<unsigned char> char_vector(bool_vector.size() / 8 + (bool_vector.size() % 8 ? 1 : 0));

    for (std::size_t i {}; i < bool_vector.size(); ++i) {
        std::size_t byte_id = i / 8;
        uint32_t bit_id = 8 - i % 8;

        unsigned char val = bool_vector[i] ? (1 << (bit_id - 1)) : 0;
        char_vector[byte_id] |= val;
    }

    return char_vector;
}

/* Parameters for encoding algorithm */

// using byte_t = uint8_t;

namespace acs {
inline void encode(std::vector<unsigned char>& input_vector, std::vector<unsigned char>& output_vector)
{
    adaptive_model model {};

    uint64_t a = 0;
    uint64_t b = WHOLE;
    uint64_t licznik = 0; // jak na wykladzie -- nie moge wymyslec lepszej nazwy

    std::vector<bool> bool_vector {};
    for (auto byte : input_vector) {

        size_t symbol = byte;
        uint64_t w = b - a;
        b = a + (w * model.scale_to(symbol) / model.occurences_sum());
        a = a + (w * model.scale_from(symbol) / model.occurences_sum());

        // skalowanie
        while (true) {
            if (b < HALF) {
                // oba po lewej stronie - rozszerzenie lewej
                bool_vector.push_back(0);
                for (auto i = decltype(licznik) {}; i < licznik; ++i) {
                    bool_vector.push_back(1);
                }
                licznik = 0;

            } else if (HALF < a) {
                // oba po prawej stronie - rozszerzenie prawej
                bool_vector.push_back(1);
                for (auto i = decltype(licznik) {}; i < licznik; ++i) {
                    bool_vector.push_back(0);
                }
                licznik = 0;

                a -= HALF;
                b -= HALF;
            } else if (QUARTER <= a && b < 3 * QUARTER) {
                // a i b sa po srodku
                a -= QUARTER;
                b -= QUARTER;
                ++licznik;
            } else {
                // a i b juz sa dluzsze niz polowa -- koniec skalowania
                break;
            }
            a *= 2;
            b *= 2;
        }

        model.update(symbol);
    }

    // skonczyly sie znaki ale przedzial musi osiagnac odpowiednia dlugosc
    licznik += 1;
    if (a < QUARTER) {
        bool_vector.push_back(0);
        for (auto i = decltype(licznik) {}; i < licznik; ++i) {
            bool_vector.push_back(1);
        }
    } else {
        bool_vector.push_back(1);
        for (auto i = decltype(licznik) {}; i < licznik; ++i) {
            bool_vector.push_back(0);
        }
    }

    output_vector = vector_cast(bool_vector);
}

inline void decode(std::vector<unsigned char>& input_vector, std::vector<unsigned char>& output_vector, std::size_t amount_to_decode)
{
    adaptive_model model {};
    output_vector.clear();

    std::vector<bool> bool_vector = vector_cast(input_vector);

    uint64_t a = 0;
    uint64_t b = WHOLE;
    uint64_t z = 0;

    
    auto iter = bool_vector.begin();
    for (size_t i = 1; i <= PRECISION; ++i) { // Initialize 'z' with as many bits as you can
        bool val = *iter;
        ++iter;
        if (val) // bit '1' read
            z += powerOf(2, PRECISION - i); // z sklada bity od najwiekszego do najmniejszego
    }

    while (true) {
        // decode a symbol (binary search)
        size_t sym_a = 0;
        size_t sym_b = model.size() - 1;
        while (sym_a <= sym_b) {
            size_t symbol = sym_a + (sym_b - sym_a) / 2;

            // jakie sa przedzialy skali jezeli left right wyznaczaja symbol
            uint64_t b_a = b - a;
            uint64_t a0 = a + b_a * model.scale_from(symbol) / model.occurences_sum();
            uint64_t b0 = a + b_a * model.scale_to(symbol) / model.occurences_sum();

            // assert(a0 < b0); // must be true
            if (a0 >= b0) {
                int a = 23;
                return;
                // throw std::runtime_error("decode:: !a < b");
            }

            // if (a0 == b0) {
            //     unsigned char decoded = static_cast<unsigned char>(symbol);
            //     output_vector.push_back(decoded);
            //     return;
            // }

            if (z < a0)
                sym_b = symbol - 1;
            else if (z >= b0)
                sym_a = symbol + 1;
            else // symbol found: a0 <= z < b0
            {
                // if (symbol == MODEL_EOF_SYMBOL) { // End Of File symbol
                //     clearProgress();
                //     return;
                // }

                unsigned char decoded = static_cast<unsigned char>(symbol);
                output_vector.push_back(decoded);

                a = a0;
                b = b0;

                model.update(decoded);
                // updateProgress((double)(bitsRead + 16) / filesize / 8);

                // if (close && a0 >= b0) {
                //     return;
                // }

                if (output_vector.size() == amount_to_decode) {
                    return;
                }

                break;
            }
        }

        // if (iter == bool_vector.end()) {
        //     return;
        // }

        // Scaling
        while (true) {
            if (b < HALF) { // rozszerzenie lewej
            } else if (HALF < a) { // rozszerzenie prawej
                a -= HALF;
                b -= HALF;
                z -= HALF;
            } else if (QUARTER <= a && b < 3 * QUARTER) { // rozszerzenie srodka
                a -= QUARTER;
                b -= QUARTER;
                z -= QUARTER;
            } else {
                // b - a juz jest wieksze niz polowa
                break;
            }
            a *= 2;
            b *= 2;
            z *= 2;

            // Update z approximation
            if (iter < bool_vector.end()) {

                if (*iter)
                    z += 1;
                ++iter;
            }
        }
    }
}
}