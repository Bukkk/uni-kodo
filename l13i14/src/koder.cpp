#include "utils/args_helper.hpp"
#include "utils/vector_streams.hpp"
#include "hamming.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>



int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    std::string in_path;
    std::string out_path;

    utils::args_helper parser {
        "program który zakodowany i zaszumiony plik in do pliku out, wypisując na końcu w ilu przypadkach napotkał 2 błędy.\n"
        "\n"
        "uzycie: program <plik in> <plik out>\n"
    };

    parser.set_positional({ .write_to = in_path });
    parser.set_positional({ .write_to = out_path });

    bool help {};
    parser.set_boolean({ .write_to = help, .symbol = "-h" });
    parser.set_boolean({ .write_to = help, .symbol = "--help" });

    if (!parser.parse(argc, argv) || help) {
        std::cout << parser.help_page();
        return 0;
    }

    using utils::vector_streams::binary::operator<<;
    using utils::vector_streams::binary::operator>>;

    std::vector<unsigned char> data_in {};
    {
        std::ifstream file { in_path };
        if (!file) {
            throw std::runtime_error { "nie mozna otworzyc pliku=" + in_path };
        }

        file >> data_in;
    }

    hamming::c_hamming_8_4 hamm {};
    std::vector<unsigned char> data_out = hamm.encode(data_in);
    {
        std::ofstream file { out_path };
        if (!file) {
            throw std::runtime_error { "nie mozna otworzyc pliku=" + out_path };
        }

        file << data_out;
    }
}