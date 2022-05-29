#include "utils/args_helper.hpp"
#include "utils/vector_streams.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>

auto count_4_blocks(std::vector<uint8_t> const& d1, std::vector<uint8_t> const& d2) -> uint64_t
{
    uint64_t result {};

    size_t size = std::min(d1.size(), d2.size());
    for (std::size_t byte {}; byte < size; ++byte) {
        uint8_t xor_val = d1[byte] ^ d2[byte];

        if (xor_val & 0x0f) {
            ++result;
        }

        if (xor_val & 0xf0) {
            ++result;
        }
    }

    return result;
}

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    std::string path1;
    std::string path2;

    utils::args_helper parser {
        "program porownujacy 2 pliki i wypisujacy ile 4bitowych blokow jest roznych\n"
        "\n"
        "uzycie: program <plik 1> <plik 2>\n"
    };

    parser.set_positional({ .write_to = path1 });
    parser.set_positional({ .write_to = path2 });

    bool help {};
    parser.set_boolean({ .write_to = help, .symbol = "-h" });
    parser.set_boolean({ .write_to = help, .symbol = "--help" });

    if (!parser.parse(argc, argv) || help) {
        std::cout << parser.help_page();
        return 0;
    }

    using utils::vector_streams::binary::operator>>;

    std::vector<unsigned char> data1 {};
    {
        std::ifstream file { path1 };
        if (!file) {
            throw std::runtime_error { "nie mozna otworzyc pliku=" + path1 };
        }

        file >> data1;
    }

    std::vector<unsigned char> data2 {};
    {
        std::ifstream file { path2 };
        if (!file) {
            throw std::runtime_error { "nie mozna otworzyc pliku=" + path2 };
        }

        file >> data2;
    }

    if (data1.size() != data2.size()) {
        throw std::runtime_error { "pliki sa roznej dlugosci. co znaczy ze bloki sie wtedy roznia?" };
    }

    uint64_t diffs = count_4_blocks(data1, data2);

    std::cout << "ilosc roznych blokow 4bitowych: " << diffs << "\n";
}