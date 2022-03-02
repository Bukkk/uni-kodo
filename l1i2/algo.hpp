#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

namespace algo {

inline auto conditional_entropy(const std::vector<unsigned char>& vec)
    -> double
{
    auto const size = vec.size();

    double conditional_entropy = 0; // ten gorny wzor
    for (uint32_t x = 0; x < 0x100; ++x) {

        // prawdopodobienstwo x to ilosc wystapien x / wsystkie znaki
        uint64_t amount = 0;
        for (size_t i = 0; i < size; ++i) {
            if (vec[i] == x) {
                ++amount;
            }
        }

        // amount_by_y[y] zawiera ilosc y wystepujacych po x
        // obliczam rownolegle bo tak jest duzo szybciej (z 13s ma 40ms)
        // poniewaz tak nie przelatuje przez pamiec 256 razy
        // oraz gwarantuje ze dokladnie jeden y zostanie dodany jezeli tylko zgadza sie poprzedni x
        // i nie trzeba opuszczac obliczen kiedy nie zgadza sie y (a potem wykonywac podobne jeszcze raz)
        std::array<uint64_t, 0x100> amount_if_last_was_x {};
        {
            auto last = (unsigned char) {};
            for (size_t i = 0; i < size; ++i) {
                auto current = vec[i];

                if (last == x) {
                    ++amount_if_last_was_x[current];
                }

                last = current;
            }
        }

        double partial_entropy = 0; // ten dolny wzor
        if (amount) {
            for (uint32_t y = 0; y < 0x100; ++y) {
                if (amount_if_last_was_x[y]) {
                    auto diff = (double)amount_if_last_was_x[y] * std::log2((double)amount / amount_if_last_was_x[y]);
                    // std::cout << diff << "\n";
                    partial_entropy += diff;
                }
            }

            partial_entropy /= amount;
        }

        std::cout << "H(Y|"
                  << (int)(unsigned char)x //<< "[" << x << "]"
                  << ") = " << partial_entropy << "\n";

        conditional_entropy += amount * partial_entropy / size;
    }
    std::cout << "H(Y|X) = " << conditional_entropy << "\n";

    return conditional_entropy;
}

inline auto entropy(const std::vector<unsigned char>& set)
    -> double
{
    auto const size = set.size();
    auto const log_size = std::log2(size);

    double entropy = 0;
    for (uint32_t x = 0; x < 0x100; ++x) {
        uint64_t amount = 0;
        for (size_t i = 0; i < size; ++i) {
            if (set[i] == x) {
                ++amount;
            }
        }

        if (amount != 0) {
            // wyciagniecie dzielenia na zewnatrz i odejmowanie zamiast dzielenia
            // osczedzilo conajwyzej kilka milisekund na 175 pan tadeusz (172 wydaje sie byc srednia)
            auto diff = amount * (log_size - std::log2(amount));
            // std::cout << diff << "\n";
            entropy += diff;
        }
    }
    entropy /= size;

    std::cout << "H(X) = " << entropy << "\n";

    return entropy;
}
}