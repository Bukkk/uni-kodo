#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

namespace algo {

inline auto conditional_entropy(const std::vector<char>& vec)
    -> double
{
    auto const size = vec.size();

    double conditional_entropy = 0; // ten gorny wzor
    for (uint32_t x = 0; x < 0x100; ++x) {

        // amount_by_y[y] zawiera ilosc y wystepujacych po x
        // obliczam rownolegle bo tak jest duzo szybciej (z 13s ma 40ms)
        // poniewaz tak nie przelatuje przez pamiec 256 razy
        // oraz gwarantuje ze dokladnie jeden y zostanie dodany jezeli tylko zgadza sie poprzedni x
        // i nie trzeba opuszczac obliczen kiedy nie zgadza sie y (a potem wykonywac podobne jeszcze raz)
        std::array<uint64_t, 0x100> y_if_last_was {};
        {
            auto last = (unsigned char) {};
            for (size_t i = 0; i < size; ++i) {
                auto current = static_cast<unsigned char>(vec[i]);

                if (last == x) {
                    ++y_if_last_was[current];
                }

                last = current;
            }
        }

        double partial_entropy = 0; // ten dolny wzor
        for (uint32_t x = 0; x < 100; ++x) {
            if (y_if_last_was[x]) {
                auto diff = (double)y_if_last_was[x] * std::log2((double)size / y_if_last_was[x]) / size;
                // std::cout << diff << "\n";
                partial_entropy += diff;
            }
        }

        std::cout << "H(Y|"
                  << (int)(unsigned char)x //<< "[" << x << "]"
                  << ") = " << partial_entropy << "\n";

        // prawdopodobienstwo x to ilosc wystapien x / wsystkie znaki
        uint64_t amount = 0;
        for (size_t i = 0; i < size; ++i) {
            if (vec[i] == x) {
                ++amount;
            }
        }

        conditional_entropy += partial_entropy * amount / size;
    }
    std::cout << "H(Y|X) = " << conditional_entropy << "\n";

    return conditional_entropy;
}

inline auto entropy(const std::vector<char>& set)
    -> double
{
    auto const size = set.size();

    double entropy;
    for (uint32_t x = 0; x < 0x100; ++x) {
        uint64_t amount = 0;
        for (size_t i = 0; i < size; ++i) {
            if (set[i] == x) {
                ++amount;
            }
        }

        if (amount != 0) {
            auto diff = (double)amount * std::log2((double)size / amount) / size;
            // std::cout << diff << "\n";
            entropy += diff;
        }
    }
    std::cout << "H(X) = " << entropy << "\n";

    return entropy;
}
}