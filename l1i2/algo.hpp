#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

namespace algo {

inline auto conditional_entropy(const std::vector<unsigned char>& set)
    -> double
{
    auto const size = set.size();

    // prawdopodobienstwo x to ilosc wystapien x / wsystkie znaki
    uint64_t amount[0x100] = {};
    for (size_t i = 0; i < size; ++i) {
        ++amount[set[i]];
    }

    // amount_by_y[y] zawiera ilosc y wystepujacych po x
    // obliczam rownolegle bo tak jest duzo szybciej (z 13s ma 40ms)
    // poniewaz tak nie przelatuje przez pamiec 256 razy
    // oraz gwarantuje ze dokladnie jeden y zostanie dodany jezeli tylko zgadza sie poprzedni x
    // i nie trzeba opuszczac obliczen kiedy nie zgadza sie y (a potem wykonywac podobne jeszcze raz)
    // std::array<uint64_t, 0x100> conditional_amount {};
    uint64_t conditional_amount[0x100][0x100] = {};
    {
        auto last = (unsigned char) {};
        for (size_t i = 0; i < size; ++i) {
            auto current = set[i];
            ++conditional_amount[last][current];
            last = current;
        }
    }

    double conditional_entropy = 0; // ten gorny wzor
    for (uint32_t x = 0; x < 0x100; ++x) {
        const auto log_x = std::log2((double)amount[x]);

        double partial_entropy = 0; // ten dolny wzor
        if (amount[x]) {
            for (uint32_t y = 0; y < 0x100; ++y) {
                if (conditional_amount[x][y]) {
                    auto diff = (double)conditional_amount[x][y] * (log_x - std::log2(conditional_amount[x][y]));
                    // std::cout << diff << "\n";
                    partial_entropy += diff;
                }
            }

            partial_entropy /= amount[x];
        }

        // std::cout << "H(Y|"
        //           << (int)(unsigned char)x //<< "[" << x << "]"
        //           << ") = " << partial_entropy << "\n";

        conditional_entropy += amount[x] * partial_entropy / size;
    }
    std::cout << "H(Y|X) = " << conditional_entropy << "\n";

    return conditional_entropy;
}

inline auto entropy(const std::vector<unsigned char>& set)
    -> double
{
    auto const size = set.size();
    auto const log_size = std::log2(size);

    uint64_t amount[0x100] = {};
    for (size_t i = 0; i < size; ++i) {
        ++amount[set[i]];
    }

    double entropy = 0;
    for (uint32_t x = 0; x < 0x100; ++x) {

        if (amount[x] != 0) {
            auto diff = amount[x] * (log_size - std::log2(amount[x]));
            // std::cout << diff << "\n";
            entropy += diff;
        }
    }
    entropy /= size;

    std::cout << "H(X) = " << entropy << "\n";

    return entropy;
}
}