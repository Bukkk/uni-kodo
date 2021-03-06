
#include "acs.hpp"

#include "utils/args_helper.hpp"
#include "utils/stream_vector.hpp"
#include "utils/time_it.hpp"

#include <chrono>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>

// Program kodujący powinien dodatkowo na koniec zwrócić na ekran odpowiednio
// entropię kodowanych danych, średnią długość kodowania i stopień kompresji.

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    utils::args_helper args { {}, { "-h", "-d", "-e" },
        "program do adaptacyjnego kodowania arytmetycznego ze skalowaniem\n"
        "\n"
        "uzycie: nazwa <plik do odczytania> <plik do zapisu> <tryb dzialania>\n"
        "\n"
        "  <plik do odczytania>\n"
        "  <plik do zapisu>\n"
        "  <tryb dzialania>                        -> czy program ma kodowac czy dekodowac\n"
        "                                             e koduj\n"
        "                                             d dekoduj\n" };

    args.parse(argc, argv);

    if (args.boolean("-h")) {
        std::cout << args.help() << '\n';
        return 0;
    }

    if (args.positional_args().size() < 3) {
        std::cout << "musisz podac argumenty pozycyjne <plik do odczytania> <plik do zapisu> <e/d>\n";
        return 1;
    }

    auto& input_filepath = args.positional_args()[0];
    auto& output_filepath = args.positional_args()[1];
    auto& mode = args.positional_args()[2];

    if (input_filepath == output_filepath) {
        std::cout << "plik zrodlowy i docelowy sa takie same\n";
        // latwo sie da oszukac ale w 80proc przypadkow zaradzi problem
        return 1;
    }

    using utils::vector_stream_operators::binary::operator<<;
    using utils::vector_stream_operators::binary::operator>>;

    // std::cout << input_data << '\n';
    // std::vector<bool> bv = vector_cast(input_data);
    // std::vector<unsigned char> cv = vector_cast(bv);
    // std::cout << cv << '\n';
    // std::cout << "org=" << input_data.size() << " bv=" << bv.size() << " cv=" << cv.size() << '\n';

    utils::time_it<std::chrono::milliseconds> timer {};

    if (mode == "e") {
        // std::cout << "koduje\n";

        std::vector<unsigned char> input_data {};
        std::ifstream input_file { input_filepath };
        if (input_file) {
            input_file >> input_data;

            input_file.close();
        } else {
            std::cout << "nie mozna otworzyc pliku czytania\n";
            return 1;
        }

        timer.set();

        auto output_data = algo::encode(input_data);

        auto millis = timer.measure();

        std::cout << "czas kodowania=" << millis << "ms\n";
        std::cout << "entropia pliku wejsciowego=" << algo::entropy(input_data) << '\n';
        std::cout << "entropia pliku wyjsciowego=" << algo::entropy(output_data) << '\n';
        std::cout << "srednia dlugosc kodowania kodu=" << algo::average_coding_length(output_data.size(), input_data.size()) << '\n';
        std::cout << "stopien kompresji kodu=" << algo::compression_ratio(output_data.size(), input_data.size()) << '\n';

        std::ofstream output_file { output_filepath };
        if (output_file) {
            uint64_t to_decode = input_data.size();
            char buff[8];
            for (uint32_t i {}; i < 8; ++i) {
                buff[i] = (to_decode >> i * 8) & 0xff;
            }
            output_file.write(buff, 8);

            output_file << output_data;

            output_file.close();
        } else {
            std::cout << "nie mozna otworzyc pliku zapisu\n";
            return 1;
        }

    } else if (mode == "d") {
        // std::cout << "dekoduje\n";

        uint64_t to_decode {};
        std::vector<unsigned char> input_data {};
        std::ifstream input_file { input_filepath };
        if (input_file) {
            char buff[8];
            input_file.read(buff, 8);
            for (uint32_t i {}; i < 8; ++i) {
                to_decode |= ((uint64_t)buff[i] & 0xff) << 8 * i;
            }

            input_file >> input_data;

            input_file.close();
        } else {
            std::cout << "nie mozna otworzyc pliku czytania\n";
            return 1;
        }

        timer.set();

        auto output_data = algo::decode(input_data, to_decode);

        auto millis = timer.measure();

//         std::cout << output_data << "<koniec pliku>---------------\n";

        std::cout << "czas dekodownia=" << millis << "ms\n";
        std::cout << "entropia pliku wejsciowego=" << algo::entropy(input_data) << '\n';
        std::cout << "entropia pliku wyjsciowego=" << algo::entropy(output_data) << '\n';
        std::cout << "srednia dlugosc kodowania kodu=" << algo::average_coding_length(input_data.size(), output_data.size()) << '\n';
        std::cout << "stopien kompresji kodu=" << algo::compression_ratio(input_data.size(), output_data.size()) << '\n';

        std::ofstream output_file { output_filepath };
        if (output_file) {
            output_file << output_data;

            output_file.close();
        } else {
            std::cout << "nie mozna otworzyc pliku zapisu\n";
            return 1;
        }
    } else {
        std::cout << "nie znany tryb dzialania\n";
        return 1;
    }
}
