#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

#include "time_it.hpp"

namespace files {

auto read(char* name)
{
    std::vector<char> vec {};

    std::ifstream file(name, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);

        vec.resize(size);
        file.read(vec.data(), size);
        file.close();
    }
    return vec;
}
}

namespace logic {

auto count_y_after_x(const std::vector<char>& vec, const char y, const char x)
{
    uint64_t count = 0;
    
    auto size = vec.size();
    if (size != 0) {
        if (vec[0] == y && x == 0) {
            ++count;
        }
        for (size_t i = 1; i < vec.size(); ++i) {
            if (vec[i] == y && vec[i - 1] == x) {
                ++count;
            }
        }
    }

    return count;
}

void run(std::vector<char> vec)
{
    auto space_size = vec.size();
    for (uint32_t j = 0; j <= 0xff; ++j) {
        char x = j;

        double h_y_of_concrete_x = 0;
        for (uint32_t i = 0; i <= 0xff; ++i) {
            char y = i;

            auto amount = count_y_after_x(vec, y, x);
            // if (amount) {
            //     std::cout << (int)(unsigned char)y << "[" << y << "]"
            //               << "|"
            //               << (int)(unsigned char)x << "[" << x << "]"
            //               << " = " << amount
            //               << "\n";
            // }

            if (amount) {
                h_y_of_concrete_x += (double)amount * std::log2((double)space_size / amount) / space_size;
            }
        }
        std::cout << "H(Y|"
                  << (int)(unsigned char)x //<< "[" << x << "]"
                  << ") = " << h_y_of_concrete_x << "\n";
        std::cout.flush();
    }
}
}

int main(int argc, char** argv)
{
    std::ios::sync_with_stdio(false);

    if (argc != 2) {
        std::cerr << "podaj plik jako argument!\n";
        return 1;
    };

    utils::time_it<std::chrono::microseconds> timer {};

    auto data = files::read(argv[1]);
    std::cout << "<info>: dlugosc: " << data.size() << "\n";

    logic::run(data);

    std::cout << "<info>: program wykonal sie w " << timer.measure() << "us";
}
