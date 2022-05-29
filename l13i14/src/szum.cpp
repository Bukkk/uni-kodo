#include "utils/args_helper.hpp"
#include "utils/misc.hpp"
#include "utils/vector_streams.hpp"

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

struct stats {
    uint64_t bits_changed {};
};

auto make_errors(std::vector<uint8_t>& data, double p) -> stats
{
    std::random_device dev {};
    std::default_random_engine eng { dev() };
    auto dist = std::uniform_real_distribution<>();

    stats s {};

    std::vector<bool> vec = utils::misc::vector_cast(data);

    for (auto&& it : vec) { // vector<bool>
        if (dist(eng) < p) {
            it = !it;
            ++s.bits_changed;
        }
    }

    data = utils::misc::vector_cast(vec);

    return s;
}

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    std::string p_str;
    std::string in_path;
    std::string out_path;

    utils::args_helper parser {
        "program tworzacy szum bledow w pliku\n"
        "\n"
        "uzycie: program <prawdopodobienstwo bitswap> <plik wejsciowy> <plik wyjsciowy>\n"
    };

    parser.set_positional({ .write_to = p_str });
    parser.set_positional({ .write_to = in_path });
    parser.set_positional({ .write_to = out_path });

    bool help {};
    parser.set_boolean({ .write_to = help, .symbol = "-h" });
    parser.set_boolean({ .write_to = help, .symbol = "--help" });

    if (!parser.parse(argc, argv) || help) {
        std::cout << parser.help_page();
        return 0;
    }

    double p {};
    {
        std::stringstream ss { p_str };
        ss >> p;
    }

    using utils::vector_streams::binary::operator<<;
    using utils::vector_streams::binary::operator>>;

    std::vector<unsigned char> data {};
    {
        std::ifstream file { in_path };
        if (!file) {
            throw std::runtime_error { "nie mozna otworzyc pliku=" + in_path };
        }

        file >> data;
    }

    stats s = make_errors(data, p);
    {
        std::ofstream file { out_path };
        if (!file) {
            throw std::runtime_error { "nie mozna otworzyc pliku=" + out_path };
        }

        file << data;
    }

    std::cout << "wprowadzono " << s.bits_changed << " bitswapow" << '\n';
}