#include <cstdint>
#include <array>
#include <cmath>
#include <iostream>
#include <vector>

namespace algo {

inline void entropia_warunkowa(const std::vector<char>& vec)
{
    auto const space_size = vec.size();

    double h_y_x = 0;
    for (uint32_t x = 0; x < 0x100; ++x) {

        // amount_by_y[y] zawiera ilosc y wystepujacych po x
        // obliczam rownolegle bo tak jest duzo szybciej (z 13s ma 40ms)
        // poniewaz tak nie przelatuje przez pamiec 256 razy
        // oraz gwarantuje ze dokladnie jeden y zostanie dodany jezeli tylko zgadza sie poprzedni x
        // i nie trzeba opuszczac obliczen kiedy nie zgadza sie y (a potem wykonywac podobne jeszcze raz)
        std::array<uint64_t, 0x100> amount_y_if_x { 0 };
        unsigned char last = 0;
        for (size_t i = 0; i < space_size; ++i) {
            unsigned char current = vec[i];

            if (last == x) {
                ++amount_y_if_x[current];
            }

            last = current;
        }

        double h_y_if_x = 0;
        for (uint32_t y = 0; y <= 0xff; ++y) {
            if (amount_y_if_x[y]) {
                auto partial_h_y_if_x = (double)amount_y_if_x[y] * std::log2((double)space_size / amount_y_if_x[y]) / space_size;
                // std::cout << partial_h_y_if_x << "\n";
                h_y_if_x += partial_h_y_if_x;
            }
        }
        std::cout << "H(Y|"
                  << (int)(unsigned char)x //<< "[" << x << "]"
                  << ") = " << h_y_if_x << "\n";
        // std::cout.flush();

        // prawdopodobienstwo x to ilosc wystapien x / wsystkie znaki
        uint64_t amount_x = 0;
        for (size_t i = 0; i < space_size; ++i) {
            if (vec[i] == x) {
                ++amount_x;
            }
        }

        h_y_x += h_y_if_x * amount_x / space_size;
    }
    std::cout << "H(Y|X) = " << h_y_x << "\n";
}

inline void entropia(const std::vector<char>& vec)
{
    auto const space_size = vec.size();

    double h_x;
    for (uint32_t x = 0; x < 0x100; ++x) {
        uint64_t amount_x = 0;
        for (size_t i = 0; i < space_size; ++i) {
            if (vec[i] == x) {
                ++amount_x;
            }
        }

        if (amount_x != 0) {
            auto partial_h = (double)amount_x * std::log2(space_size / amount_x) / space_size;
            // std::cout << partial_h << "\n";
            h_x += partial_h;
        }
    }
    std::cout << "H(X) = " << h_x << "\n";
}
}